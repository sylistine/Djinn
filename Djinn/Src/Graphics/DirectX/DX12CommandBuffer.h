#pragma once

#include <wrl.h>

#include <d3d12.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include "D3DUtil.h"

#include "DxException.h"
#include "DX12RHI.h"
#include "DX12MeshData.h"
#include "../CommandBuffer.h"

#include "../../Math.h"

namespace Djinn
{
    /*struct ObjectConstants
    {
        XMFLOAT4X4 WorldViewProj = Math::Identity();
    };*/

    class DX12CommandBuffer : public CommandBuffer {
    public:
        DX12CommandBuffer(DX12RHI *dx12rhi, ID3D12Device *device, D3D12_VIEWPORT viewport, D3D12_RECT scissorRect);
        ~DX12CommandBuffer() override;

        void Initialize()override;
        void Draw()override;
        void UpdateViewMatrix(Transform *cameraTransform)override;
        void SetStaticGeometry(Vertex *verts, UINT vertc, UINT *indices, UINT indexc)override;
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

        std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;
        DX12MeshData *staticGeo = nullptr;

        // TODO: review these
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvHeap = nullptr;

        //std::unique_ptr<UploadBuffer<ObjectConstants>> objectCB = nullptr;

        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

        Microsoft::WRL::ComPtr<ID3DBlob> vertShader;
        Microsoft::WRL::ComPtr<ID3DBlob> fragShader;

        Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState = nullptr;

        void BuildDescriptorHeaps();
        void BuildConstantBuffers();
        void BuildRootSignature();
        void BuildShadersAndInputLayout();
        void BuildInitialGeometry();
        void BuildPSO();
        // END TODO

        void UploadResourceBuffers(ID3D12Resource *resource, const void *data, UINT dataSizeInBytes);
    };
}
