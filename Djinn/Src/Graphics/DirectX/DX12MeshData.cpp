#include "DX12MeshData.h"

using namespace Djinn;

DX12MeshData::DX12MeshData(
    ID3D12Device *device,
    Vertex *verts, UINT vertc,
    UINT *indices, UINT indexc)
    : vertexCount(vertc)
    , indexCount(indexc)
{
    const UINT vbSizeInBytes = vertc * sizeof(Vertex);
    const UINT ibSizeInBytes = indexc * sizeof(UINT);

    ThrowIfFailed(FINFO, D3DCreateBlob(vbSizeInBytes, &vertexBlob));
    CopyMemory(vertexBlob->GetBufferPointer(), verts, vbSizeInBytes);

    ThrowIfFailed(FINFO, D3DCreateBlob(ibSizeInBytes, &indexBlob));
    CopyMemory(indexBlob->GetBufferPointer(), indices, ibSizeInBytes);

    ThrowIfFailed(FINFO, device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(vbSizeInBytes),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(vertexBuffer.GetAddressOf())
    ));

    ThrowIfFailed(FINFO, device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(ibSizeInBytes),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(indexBuffer.GetAddressOf())
    ));

    vbv.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vbv.SizeInBytes = vbSizeInBytes;
    vbv.StrideInBytes = sizeof(Vertex);

    ibv.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    ibv.SizeInBytes = ibSizeInBytes;
    ibv.Format = DXGI_FORMAT_R16_UINT;
}


DX12MeshData::DX12MeshData(const DX12MeshData& other)
{

}


DX12MeshData::~DX12MeshData()
{

}


D3D12_VERTEX_BUFFER_VIEW DX12MeshData::VertexBufferView()const
{
    return vbv;
}


D3D12_INDEX_BUFFER_VIEW DX12MeshData::IndexBufferView()const
{
    return ibv;
}


ID3D12Resource *DX12MeshData::VertexBuffer()const
{
    return vertexBuffer.Get();
}


ID3D12Resource *DX12MeshData::IndexBuffer()const
{
    return indexBuffer.Get();
}


UINT DX12MeshData::VertexCount()const
{
    return vertexCount;
}


UINT DX12MeshData::IndexCount()const
{
    return indexCount;
}
