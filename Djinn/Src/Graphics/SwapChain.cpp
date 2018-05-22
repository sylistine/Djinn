#include "SwapChain.h"


SwapChain::SwapChain(const GfxDevice& gfxDevice)
{
    /*
    ThrowIfFailed(FINFO, CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));

    int width, height;
    gfxDevice.ClientParams(width, height);

    DXGI_MODE_DESC bufferDesc;
    bufferDesc.Width = width;
    bufferDesc.Height = height;
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = gfxDevice.BackBufferFormat();
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc = sampleDesc;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = swapChainBufferCount;
    swapChainDesc.OutputWindow = gfxDevice.HWnd();
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    ThrowIfFailed(FINFO, dxgiFactory->CreateSwapChain(
        commandQueue.Get(),
        &swapChainDesc,
        swapChain.GetAddressOf()));
    */
}


SwapChain::~SwapChain()
{
}


ID3D12Resource *SwapChain::CurrentBackBuffer()const
{
    return swapChainBuffer[currentBackBuffer].Get();
}


void SwapChain::PresentAndFlip()
{
    ThrowIfFailed(FINFO, swapChain->Present(0, 0));
    currentBackBuffer = (currentBackBuffer + 1) % swapChainBufferCount;
}
