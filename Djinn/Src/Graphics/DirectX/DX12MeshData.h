#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <d3dcompiler.h>

#include "DxException.h"
#include "d3dx12.h"
#include "../../Vertex.h"

namespace Djinn
{
    class DX12MeshData
    {
    public:
        DX12MeshData(ID3D12Device *device,
                     Vertex *verts, UINT vertc,
                     UINT *indices, UINT indexc);
        DX12MeshData(const DX12MeshData& other);
        ~DX12MeshData();

        D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const;
        D3D12_INDEX_BUFFER_VIEW IndexBufferView()const;
        ID3D12Resource *VertexBuffer()const;
        ID3D12Resource *IndexBuffer()const;
        UINT VertexCount()const;
        UINT IndexCount()const;
    private:
        Microsoft::WRL::ComPtr<ID3DBlob> vertexBlob = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> indexBlob = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer = nullptr;

        D3D12_VERTEX_BUFFER_VIEW vbv;
        D3D12_INDEX_BUFFER_VIEW ibv;
        UINT vertexCount = 0;
        UINT indexCount = 0;
    };
}
