#pragma once

#include <wrl.h>

#include <d3d12.h>
#include "d3dx12.h"

#include "DxException.h"
#include "../CommandBuffer.h"

namespace Djinn
{
    class DX12CommandBuffer : public CommandBuffer {
    public:
        DX12CommandBuffer(ID3D12Device *device, int clientWidth, int clientHeight);
        ~DX12CommandBuffer() override;
        void Reset()override;
        void Clear(float *clearColor)override;
        void UpdateClientSize(int width, int height);
    private:
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> directCommandListAlloc;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
        ID3D12Device *device;
        D3D12_VIEWPORT screenViewport;
        D3D12_RECT scissorRect;
    };
}