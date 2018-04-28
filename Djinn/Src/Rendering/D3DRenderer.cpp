#include "D3DRenderer.h"


D3DRenderer::D3DRenderer(HWND hWnd, int width, int height) :
    hWnd(hWnd), clientWidth(width), clientHeight(height) { }


D3DRenderer::~D3DRenderer() { }


bool D3DRenderer::Initialize()
{

#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
    debugController->EnableDebugLayer();
#endif

    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));

    HRESULT deviceCreateResult = D3D12CreateDevice(
        nullptr,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&device));
    if (FAILED(deviceCreateResult))
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));
    }

    ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

    rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    cbvSrvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = backBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;
    ThrowIfFailed(device->CheckFeatureSupport(
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &msQualityLevels,
        sizeof msQualityLevels));

    msaa4xQuality = msQualityLevels.NumQualityLevels;
    if (msaa4xQuality <= 0) return false;

#if _DEBUG
    LogAdapters();
#endif

    CreateCommandObjects();
    CreateSwapChain();
    CreateRtvAndDsvDescriptorHeaps();

    // Run this code once after initialization.
    OnResize();

    initialized = true;
    return true;
}


void D3DRenderer::OnResize()
{
    FlushCommandQueue();
    ThrowIfFailed(commandList->Reset(directCommandListAlloc.Get(), nullptr));
    for (auto& i : swapChainBuffer)
    {
        i.Reset();
    }
    depthStencilBuffer.Reset();

    ThrowIfFailed(swapChain->ResizeBuffers(
        swapChainBufferCount,
        clientWidth, clientHeight,
        backBufferFormat,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
    ));

    currentBackBuffer = 0;

    auto rtvHeapHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < swapChainBufferCount; ++i)
    {
        ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainBuffer[i])));
        device->CreateRenderTargetView(
            swapChainBuffer[i].Get(),
            nullptr,
            rtvHeapHandle);
        rtvHeapHandle.ptr += rtvDescriptorSize;
    }

    // Build depth stencil view
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = clientWidth;
    depthStencilDesc.Height = clientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthStencilDesc.SampleDesc.Count = msaa4xState ? 4 : 1;
    depthStencilDesc.SampleDesc.Quality = msaa4xState ? msaa4xQuality - 1 : 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = depthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;

    D3D12_HEAP_PROPERTIES depthStencilHeapProperties;
    depthStencilHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    depthStencilHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    depthStencilHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    depthStencilHeapProperties.CreationNodeMask = 1;
    depthStencilHeapProperties.VisibleNodeMask = 1;

    ThrowIfFailed(device->CreateCommittedResource(
        &depthStencilHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &optClear,
        IID_PPV_ARGS(depthStencilBuffer.GetAddressOf())
    ));

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = depthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;
    device->CreateDepthStencilView(
        depthStencilBuffer.Get(),
        &dsvDesc,
        dsvHeap->GetCPUDescriptorHandleForHeapStart());

    // Prepare CommandList and CommandQueue
    D3D12_RESOURCE_BARRIER commandListResourceBarrier;
    commandListResourceBarrier.Transition.pResource = depthStencilBuffer.Get();
    commandListResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    commandListResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    commandListResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &commandListResourceBarrier);

    ThrowIfFailed(commandList->Close());
    ID3D12CommandList *commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    FlushCommandQueue();

    // Update viewport and scissor rect.
    screenViewport.TopLeftX = 0;
    screenViewport.TopLeftY = 0;
    screenViewport.Width = static_cast<float>(clientWidth);
    screenViewport.Height = static_cast<float>(clientHeight);
    screenViewport.MinDepth = 0.0f;
    screenViewport.MaxDepth = 1.0f;

    scissorRect = { 0, 0, clientWidth, clientHeight };
}


bool D3DRenderer::GetMsaa4xState()
{
    return msaa4xState;
}


void D3DRenderer::SetMsaa4xState(bool newState)
{
    msaa4xState = newState;
}


void D3DRenderer::SetClientDimensions(int width, int height)
{
    clientWidth = width;
    clientHeight = height;
    OnResize();
}


/// Blocks until GPU finishes handling commands.
void D3DRenderer::FlushCommandQueue()
{
    currentFence++;

    ThrowIfFailed(commandQueue->Signal(fence.Get(), currentFence));

    if (fence->GetCompletedValue() < currentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
        ThrowIfFailed(fence->SetEventOnCompletion(currentFence, eventHandle));
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}


bool D3DRenderer::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));

    ThrowIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(directCommandListAlloc.GetAddressOf()))
    );

    ThrowIfFailed(device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        directCommandListAlloc.Get(),
        nullptr,
        IID_PPV_ARGS(commandList.GetAddressOf()))
    );

    commandList->Close();

    return true;
}


void D3DRenderer::CreateSwapChain()
{
    swapChain.Reset();

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swapChainDesc.BufferDesc.Width = clientWidth;
    swapChainDesc.BufferDesc.Height = clientHeight;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = backBufferFormat;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = msaa4xState ? 4 : 1;
    swapChainDesc.SampleDesc.Quality = msaa4xState ? (msaa4xQuality - 1) : 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = swapChainBufferCount;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    ThrowIfFailed(dxgiFactory->CreateSwapChain(
        commandQueue.Get(),
        &swapChainDesc,
        swapChain.GetAddressOf()));
}


void D3DRenderer::CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = swapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    ThrowIfFailed(device->CreateDescriptorHeap(
        &rtvHeapDesc, IID_PPV_ARGS(rtvHeap.GetAddressOf()))
    );

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    ThrowIfFailed(device->CreateDescriptorHeap(
        &dsvHeapDesc, IID_PPV_ARGS(dsvHeap.GetAddressOf()))
    );
}


void D3DRenderer::LogAdapters()
{
    UINT i = 0;
    IDXGIAdapter *adapter = nullptr;
    std::vector<IDXGIAdapter*> adapters;
    while(dxgiFactory->EnumAdapters(i++, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        std::wstring text = L"Adapter: ";
        text += desc.Description;
        text += L"\n";

        OutputDebugString(text.c_str());

        LogAdapterOutputs(adapter);

        adapters.push_back(adapter);
    }

    for (auto& x : adapters)
    {
        x->Release();
        x = nullptr;
    }
}


void D3DRenderer::LogAdapterOutputs(IDXGIAdapter* adapter)
{
    UINT i = 0;
    IDXGIOutput* output = nullptr;
    while (adapter->EnumOutputs(i++, &output) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);

        std::wstring text = L"Output: ";
        text += desc.DeviceName;
        text += L"\n";

        OutputDebugString(text.c_str());

        LogOutputDisplayModes(output, backBufferFormat);

        output->Release();
        output = nullptr;
    }
}


void D3DRenderer::LogOutputDisplayModes(IDXGIOutput* output, const DXGI_FORMAT format)
{
    UINT flags = 0;

    UINT count = 0;
    output->GetDisplayModeList(format, flags, &count, nullptr);
    auto modeList = new DXGI_MODE_DESC[count];
    output->GetDisplayModeList(format, flags, &count, modeList);

    for (UINT i = 0; i < count; ++i)
    {
        UINT num = modeList[i].RefreshRate.Numerator;
        UINT den = modeList[i].RefreshRate.Denominator;
        wstring text =
            to_wstring(modeList[i].Width) + L"x" +
            to_wstring(modeList[i].Height) + L"@" +
            to_wstring(num) + L"/" + to_wstring(den) + L"hz\n";

        OutputDebugString(text.c_str());
    }
}

