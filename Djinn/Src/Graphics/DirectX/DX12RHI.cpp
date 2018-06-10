#include "DX12RHI.h"

#include "DxException.h"

#include "../../Timer.h"

using namespace Djinn;
using namespace Microsoft::WRL;

DX12RHI::DX12RHI(HWND hWnd, int width, int height)
    : hWnd(hWnd)
    , clientWidth(width)
    , clientHeight(height)
    , screenViewport()
    , scissorRect()
{ }


DX12RHI::~DX12RHI()
{
    if (device != nullptr)
    {
        FlushCommandQueue();
    }
}


MSAA_SAMPLE_LEVEL DX12RHI::GetMsaaSampleLevel()
{
    return msaaSampleLevel;
}


void DX12RHI::SetMsaaSampleLevel(MSAA_SAMPLE_LEVEL newLevel)
{
    if (msaaSampleLevel != newLevel) {
        msaaSampleLevel = newLevel;
        CreateSwapChain();
        OnResize();
    }
}


bool DX12RHI::IsInitialized()
{
    return initialized;
}


void DX12RHI::SetClientDimensions(int width, int height)
{
    clientWidth = width;
    clientHeight = height;
    OnResize();
}


ID3D12Resource *DX12RHI::CurrentBackBuffer()const
{
    return swapChainTexture[currentBackBuffer].Get();
}


D3D12_CPU_DESCRIPTOR_HANDLE DX12RHI::CurrentBackBufferView()const
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle;
    handle.ptr = rtvHeap->GetCPUDescriptorHandleForHeapStart().ptr +
        currentBackBuffer * rtDescriptorSize;
    return handle;
}


D3D12_CPU_DESCRIPTOR_HANDLE DX12RHI::DepthStencilView()const
{
    return dsvHeap->GetCPUDescriptorHandleForHeapStart();
}


bool DX12RHI::Initialize()
{
#ifdef _DEBUG
    // Init debug layer
    ComPtr<ID3D12Debug> debugController;
    ThrowIfFailed(FINFO, D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
    debugController->EnableDebugLayer();
#endif

    // Create primary D3D resources.
    // dxgiFactory, virtual device, and fence
    ThrowIfFailed(FINFO, CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));

    auto hresult = D3D12CreateDevice(
        nullptr,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&device)
    );

    if (FAILED(hresult))
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(FINFO, dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        ThrowIfFailed(FINFO, D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&device)));
    }

#if _DEBUG
    LogAdapters();
#endif

    ThrowIfFailed(FINFO, device->CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&fence)
    ));

    UpdateMSAASupport();

    CreateCommandObjects();
    CreateSwapChain();

    CreateRtvAndDsvDescriptorHeaps();

    SetupPipeline();

    OnResize();

    initialized = true;
    return true;
}


/// The fun stuff!
void DX12RHI::PrepareMainCommandBuffer()
{
    // Getting the clearcolor.
    // This is just for fun and won't live here much longer.
    static float hue = 0.f;
    float speed = 10.f; // h increment per second.
    float r, g, b;
    Color::Hsv2Rgb(hue, 1.f, 1.f, r, g, b);
    float clearColor[4] = { r, g, b, 1.0f };
    hue += speed * Timer::GetTimer()->DeltaTime();
    hue = fmod(hue, 360.f);

    ThrowIfFailed(FINFO, directCommandAllocator->Reset());

    ThrowIfFailed(FINFO, commandList->Reset(directCommandAllocator.Get(), nullptr));

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    commandList->RSSetViewports(1, &screenViewport);
    commandList->RSSetScissorRects(1, &scissorRect);
    // Clear to black, baby.
    commandList->ClearRenderTargetView(
        CurrentBackBufferView(),
        clearColor,
        0, nullptr);
    commandList->ClearDepthStencilView(
        DepthStencilView(),
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        1.0f, 0, 0, nullptr);

    commandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    ThrowIfFailed(FINFO, commandList->Close());

    ID3D12CommandList *commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, commandLists);

    ThrowIfFailed(FINFO, swapChain->Present(0, 0));
    currentBackBuffer = (currentBackBuffer + 1) % swapChainBufferCount;

    FlushCommandQueue();
}


void DX12RHI::OnResize()
{
    FlushCommandQueue();

    // Reset com ptrs.
    ThrowIfFailed(FINFO, commandList->Reset(directCommandAllocator.Get(), nullptr));
    for (auto& i : swapChainTexture)
    {
        i.Reset();
    }
    depthStencilTexture.Reset();

    ThrowIfFailed(FINFO, swapChain->ResizeBuffers(
        swapChainBufferCount,
        clientWidth, clientHeight,
        backBufferFormat,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
    ));

    currentBackBuffer = 0;

    auto rtvHeapHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < swapChainBufferCount; ++i)
    {
        ThrowIfFailed(FINFO, swapChain->GetBuffer(
            i,
            IID_PPV_ARGS(&swapChainTexture[i])
        ));
        
        device->CreateRenderTargetView(
            swapChainTexture[i].Get(),
            nullptr,
            rtvHeapHandle);
        rtvHeapHandle.ptr += rtDescriptorSize;
    }

    // Build depth stencil view
    D3D12_HEAP_PROPERTIES depthStencilHeapProperties;
    depthStencilHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    depthStencilHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    depthStencilHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    depthStencilHeapProperties.CreationNodeMask = 1;
    depthStencilHeapProperties.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = clientWidth;
    depthStencilDesc.Height = clientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthStencilDesc.SampleDesc = GetSampleDescriptor();
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = depthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;

    ThrowIfFailed(FINFO, device->CreateCommittedResource(
        &depthStencilHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &optClear,
        IID_PPV_ARGS(depthStencilTexture.GetAddressOf())
    ));

    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDescriptor;
    depthStencilDescriptor.Flags = D3D12_DSV_FLAG_NONE;
    depthStencilDescriptor.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    depthStencilDescriptor.Format = depthStencilFormat;
    depthStencilDescriptor.Texture2D.MipSlice = 0;
    device->CreateDepthStencilView(
        depthStencilTexture.Get(),
        &depthStencilDescriptor,
        dsvHeap->GetCPUDescriptorHandleForHeapStart()
    );

    commandList->ResourceBarrier(
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            depthStencilTexture.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_DEPTH_WRITE
        )
    );

    ThrowIfFailed(FINFO, commandList->Close());
    ID3D12CommandList *commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, commandLists);

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


/// Blocks until GPU finishes handling commands.
void DX12RHI::FlushCommandQueue()
{
    currentFence++;

    ThrowIfFailed(FINFO, commandQueue->Signal(fence.Get(), currentFence));

    if (fence->GetCompletedValue() < currentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
        ThrowIfFailed(FINFO, fence->SetEventOnCompletion(currentFence, eventHandle));
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}


void DX12RHI::UpdateMSAASupport()
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = backBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;
    ThrowIfFailed(FINFO, device->CheckFeatureSupport(
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &msQualityLevels, sizeof msQualityLevels));
    msaa4xMaxQuality = msQualityLevels.NumQualityLevels - 1;
    msQualityLevels.SampleCount = 8;
    ThrowIfFailed(FINFO, device->CheckFeatureSupport(
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &msQualityLevels, sizeof msQualityLevels));
    msaa8xMaxQuality = msQualityLevels.NumQualityLevels - 1;
}


bool DX12RHI::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(FINFO, device->CreateCommandQueue(
        &queueDesc,
        IID_PPV_ARGS(&commandQueue)
    ));

    ThrowIfFailed(FINFO, device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(directCommandAllocator.GetAddressOf())
    ));

    ThrowIfFailed(FINFO, device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        directCommandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(commandList.GetAddressOf())
    ));

    commandList->Close();

    return true;
}


inline DXGI_SAMPLE_DESC DX12RHI::GetSampleDescriptor()const
{
    UINT count;
    UINT quality;
    switch (msaaSampleLevel) {
    case MSAA_SAMPLE_LEVEL_4X:
        count = 4;
        quality = msaa4xMaxQuality;
        break;
    case MSAA_SAMPLE_LEVEL_8X:
        count = 8;
        quality = msaa8xMaxQuality;
        break;
    default:
        count = 1;
        quality = 0;
        break;
    }

    DXGI_SAMPLE_DESC sampleDesc;
    sampleDesc.Count = count;
    sampleDesc.Quality = quality;
    return sampleDesc;
}


void DX12RHI::CreateSwapChain()
{
    swapChain.Reset();

    DXGI_MODE_DESC bufferDesc;
    bufferDesc.Width = clientWidth;
    bufferDesc.Height = clientHeight;
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = backBufferFormat;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc = GetSampleDescriptor();
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = swapChainBufferCount;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    ThrowIfFailed(FINFO, dxgiFactory->CreateSwapChain(
        commandQueue.Get(),
        &swapChainDesc,
        swapChain.GetAddressOf()));
}


void DX12RHI::CreateRtvAndDsvDescriptorHeaps()
{
    rtDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = swapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    ThrowIfFailed(FINFO, device->CreateDescriptorHeap(
        &rtvHeapDesc,
        IID_PPV_ARGS(rtvHeap.GetAddressOf())
    ));

    dsDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    D3D12_DESCRIPTOR_HEAP_DESC dsHeapDesc;
    dsHeapDesc.NumDescriptors = 1;
    dsHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsHeapDesc.NodeMask = 0;
    ThrowIfFailed(FINFO, device->CreateDescriptorHeap(
        &dsHeapDesc,
        IID_PPV_ARGS(dsvHeap.GetAddressOf())
    ));

    cbvSrvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}


void DX12RHI::SetupPipeline() {
    D3D12_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        {
            "POSITION", 0,
            DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
        {
            "COLOR", 0,
            DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        }
    };

    ComPtr<ID3D12Resource> staticVertexBuffer;
    ThrowIfFailed(FINFO, device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(24),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(staticVertexBuffer.GetAddressOf())
    ));
    ComPtr<ID3D12Resource> staticVertexUploadBuffer;
    ThrowIfFailed(FINFO, device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(24),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(staticVertexUploadBuffer.GetAddressOf())
    ));

    D3D12_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pData = nullptr;
    subresourceData.RowPitch = 24;
    subresourceData.SlicePitch = 24;

}


void DX12RHI::LogAdapters()
{
    UINT i = 0;
    IDXGIAdapter *adapter = nullptr;
    std::vector<IDXGIAdapter*> adapters;
    while (dxgiFactory->EnumAdapters(i++, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        std::wstring text = L"Adapter: ";
        text += desc.Description;
        text += L"\n";

        OutputDebugString(text.c_str());

        adapters.push_back(adapter);
    }

    for (auto& x : adapters)
    {
        LogAdapterOutputs(x);
        x->Release();
        x = nullptr;
    }
}


void DX12RHI::LogAdapterOutputs(IDXGIAdapter* adapter)
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


void DX12RHI::LogOutputDisplayModes(IDXGIOutput* output, const DXGI_FORMAT format)
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
        std::wstring text =
            std::to_wstring(modeList[i].Width) + L"x" +
            std::to_wstring(modeList[i].Height) + L"@" +
            std::to_wstring(num) + L"/" + std::to_wstring(den) + L"hz\n";

        OutputDebugString(text.c_str());
    }
}

