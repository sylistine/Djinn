#pragma once

#include <wrl.h>

#include <d3d12.h>
#include <DirectXMath.h>
#include "d3dx12.h"

#include "DxException.h"

#include "DX12RHI.h"
#include "../CommandBuffer.h"
#include "../../Math.h"

namespace Djinn
{
    class DX12CommandBuffer : public CommandBuffer {
    public:
        DX12CommandBuffer(DX12RHI *dx12rhi, ID3D12Device *device);
        ~DX12CommandBuffer() override;

        void Initialize()override;
        void Draw()override;
        void UpdateViewMatrix(Transform *cameraTransform)override;
    private:
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> directCommandListAlloc;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
        DX12RHI *dx12rhi;
        ID3D12Device *device;
        D3D12_VIEWPORT screenViewport;
        D3D12_RECT scissorRect;
        DirectX::XMFLOAT4X4 worldMatrix = Math::Identity();
        DirectX::XMFLOAT4X4 viewMatrix = Math::Identity();
        DirectX::XMFLOAT4X4 projectionMatrix = Math::Identity();

        void BuildDescriptorHeaps();
        void BuildConstantBuffers();
        void BuildRootSignature();
        void BuildShadersAndInputLayout();
        void BuildBoxGeometry();
        void BuildPSO();
    };
}
