#pragma once

#include <wrl.h>

#include <dxgi1_4.h>
#include <d3d12.h>

#include "../SwapChain.h"

namespace Djinn {
    class DX12SwapChain : public SwapChain {
    public:
        DX12SwapChain(int width, int height, DXGI_FORMAT bufferFormat);
        //DX12SwapChain(const DX12SwapChain& other);
        ~DX12SwapChain();
        void Reset();
        ID3D12Resource * CurrentBackBuffer()const;
        D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
    private:
        static const int swapChainBufferCount = 2;
        int currentBackBuffer = 0;
        Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
        Microsoft::WRL::ComPtr<ID3D12Resource> swapChainBuffer[swapChainBufferCount];
    };
}