#include "D3DRenderer.h"
#include "../Utils.h"


D3DRenderer::D3DRenderer(HWND hWnd, int width, int height) :
    hWnd(hWnd), width(width), height(height) { }


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

    return true;
}


void D3DRenderer::OnResize()
{
    
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
    
}



bool D3DRenderer::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    HRESULT result = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
    if (FAILED(result)) return false;

    result = device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(directCommandListAlloc.GetAddressOf()));
    if (FAILED(result)) return false;

    result = device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        directCommandListAlloc.Get(),
        nullptr,
        IID_PPV_ARGS(commandList.GetAddressOf()));
    if (FAILED(result)) return false;

    commandList->Close();

    return true;
}


bool D3DRenderer::CreateSwapChain()
{
    swapChain.Reset();

    DXGI_SWAP_CHAIN_DESC scDesc;
    scDesc.BufferDesc.Width = width;
    scDesc.BufferDesc.Height = height;
    scDesc.BufferDesc.RefreshRate.Numerator = 60;
    scDesc.BufferDesc.RefreshRate.Denominator = 1;
    scDesc.BufferDesc.Format = backBufferFormat;
    scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    scDesc.SampleDesc.Count = msaa4xState ? 4 : 1;
    scDesc.SampleDesc.Quality = msaa4xState ? (msaa4xQuality - 1) : 1;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.BufferCount = swapChainBufferCount;
    scDesc.OutputWindow = hWnd;
    scDesc.Windowed = true;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    HRESULT result = dxgiFactory->CreateSwapChain(
        commandQueue.Get(),
        &scDesc,
        swapChain.GetAddressOf());
    return !FAILED(result);
}

bool D3DRenderer::CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = swapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    HRESULT result = device->CreateDescriptorHeap(
        &rtvHeapDesc, IID_PPV_ARGS(rtvHeap.GetAddressOf()));
    if (FAILED(result)) return false;

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    result = device->CreateDescriptorHeap(
        &dsvHeapDesc, IID_PPV_ARGS(dsvHeap.GetAddressOf()));
    return !FAILED(result);
}


void D3DRenderer::LogAdapters()
{
    // TODO: this.
}

