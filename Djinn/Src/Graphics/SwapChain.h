#pragma once


#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>

#include "DirectX/DxException.h"

#include "GfxDevice.h"


class SwapChain
{
public:
    SwapChain(const GfxDevice& gfxDevice);
    ~SwapChain();
    ID3D12Resource *CurrentBackBuffer() const;
    void PresentAndFlip();
private:
    static const int swapChainBufferCount = 2;
    Microsoft::WRL::ComPtr<IDXGIFactory1> dxgiFactory;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D12Resource> swapChainBuffer[swapChainBufferCount];
    int currentBackBuffer = 0;
};

