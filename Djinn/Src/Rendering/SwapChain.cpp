#include "SwapChain.h"
#include "Msaa4xState.h"


SwapChain::SwapChain()
{
}


SwapChain::~SwapChain()
{
}


void SwapChain::Create(HWND hWnd, UINT width, UINT height, Msaa4xState msaaState)
{
    swapChain.Reset();

    DXGI_MODE_DESC bufferDesc;
    bufferDesc.Width = width;
    bufferDesc.Height = height;
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = backBufferFormat;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SAMPLE_DESC sampleDesc;
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;
    //swapChainDesc.SampleDesc.Count = msaaState.Is4x() ? 4 : 1;
    //swapChainDesc.SampleDesc.Quality = msaaState.Is4x() ? (msaaState.quality - 1) : 0;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc = sampleDesc;
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