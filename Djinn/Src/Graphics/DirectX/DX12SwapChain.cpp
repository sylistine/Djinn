#include "DX12SwapChain.h"


using namespace Djinn;
using namespace Microsoft::WRL;


DX12SwapChain::DX12SwapChain(int width, int height, DXGI_FORMAT bufferFormat)
{
    DXGI_MODE_DESC bufferDesc;
    bufferDesc.Width = width;
    bufferDesc.Height = height;
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = bufferFormat;
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

DX12SwapChain::~DX12SwapChain()
{

}


void DX12SwapChain::Reset()
{
    for (auto& i : swapChainBuffer)
    {
        i.Reset();
    }

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
        ThrowIfFailed(FINFO, swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainBuffer[i])));
        device->CreateRenderTargetView(
            swapChainBuffer[i].Get(),
            nullptr,
            rtvHeapHandle);
        rtvHeapHandle.ptr += rtvDescriptorSize;
    }
}


ID3D12Resource *DX12SwapChain::CurrentBackBuffer()const
{
    return swapChainBuffer[currentBackBuffer].Get();
}


D3D12_CPU_DESCRIPTOR_HANDLE DX12SwapChain::CurrentBackBufferView()const
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle;
    handle.ptr = rtvHeap->GetCPUDescriptorHandleForHeapStart().ptr +
        currentBackBuffer * rtvDescriptorSize;
    return handle;
}