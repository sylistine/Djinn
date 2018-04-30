#pragma once
//#include <Windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>

#include "../Utils.h"

using namespace Microsoft::WRL;

class SwapChain
{
public:
    SwapChain();
    virtual ~SwapChain();
    void Create();
private:
    static const int swapChainBufferCount = 2;
    int currentBackBuffer;
    ComPtr<IDXGISwapChain> swapChain;
    ComPtr<ID3D12Resource> swapChainBuffer[swapChainBufferCount];
};

