#include "DX12CommandBuffer.h"

#include <fstream>

#include "../../Color.h"
#include "../../Timer.h"

using namespace Djinn;
using namespace DirectX;

DX12CommandBuffer::DX12CommandBuffer(DX12RHI *dx12rhi, ID3D12Device *device,
    D3D12_VIEWPORT viewport, D3D12_RECT scissorRect)
    : dx12rhi(dx12rhi)
    , device(device)
    , screenViewport(viewport)
    , scissorRect(scissorRect)
{
    ThrowIfFailed(FINFO, device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(directCommandListAlloc.GetAddressOf())
    ));

    ThrowIfFailed(FINFO, device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        directCommandListAlloc.Get(),
        nullptr,
        IID_PPV_ARGS(commandList.GetAddressOf())
    ));

    commandList->Close();
}


DX12CommandBuffer::~DX12CommandBuffer()
{
}

void DX12CommandBuffer::Initialize()
{
    // Reset the command list to prep for initialization commands.
    ThrowIfFailed(FINFO, commandList->Reset(directCommandListAlloc.Get(), nullptr));

    BuildDescriptorHeaps();
    BuildConstantBuffers();
    BuildRootSignature();
    BuildShadersAndInputLayout();
    BuildInitialGeometry();
    BuildPSO();

    // Execute the initialization commands.
    ThrowIfFailed(FINFO, commandList->Close());

    dx12rhi->SubmitCommandList(commandList.Get());
    dx12rhi->ExecuteCommandLists();
    dx12rhi->FlushCommandQueue();
}

void DX12CommandBuffer::Draw()
{
    // Getting the clearcolor.
    // This is just for fun and won't live here much longer.
    static float hue = 0.f;
    float speed = 24.f; // h increment per second.
    float r, g, b;
    Color::Hsv2Rgb(hue, 0.25f, 0.75f, r, g, b);
    float clearColor[4] = { r, g, b, 1.0f };
    hue += speed * Timer::GetTimer()->DeltaTime();
    hue = fmod(hue, 360.f);

    ThrowIfFailed(FINFO, directCommandListAlloc->Reset());
    ThrowIfFailed(FINFO, commandList->Reset(
        directCommandListAlloc.Get(), pipelineState.Get()
    ));

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        dx12rhi->CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    ));

    commandList->RSSetViewports(1, &screenViewport);
    commandList->RSSetScissorRects(1, &scissorRect);
    
    commandList->ClearRenderTargetView(
        dx12rhi->CurrentBackBufferView(),
        clearColor,
        0, nullptr);

    commandList->ClearDepthStencilView(
        dx12rhi->DepthStencilView(),
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        1.0f, 0, 0, nullptr);

    commandList->OMSetRenderTargets(
        1,
        &dx12rhi->CurrentBackBufferView(),
        true,
        &dx12rhi->DepthStencilView()
    );

    //ID3D12DescriptorHeap* descHeaps[] = { cbvHeap.Get() };
    //commandList->SetDescriptorHeaps(1, descHeaps);

    commandList->SetGraphicsRootSignature(rootSignature.Get());

    commandList->IASetVertexBuffers(0, 1, &staticGeo->VertexBufferView());
    commandList->IASetIndexBuffer(&staticGeo->IndexBufferView());
    commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //commandList->SetGraphicsRootDescriptorTable(0, cbvHeap->GetGPUDescriptorHandleForHeapStart());

    commandList->DrawIndexedInstanced(
        staticGeo->IndexCount(),
        1, 0, 0, 0
    );

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        dx12rhi->CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    ));

    ThrowIfFailed(FINFO, commandList->Close());

    dx12rhi->SubmitCommandList(commandList.Get());
    dx12rhi->ExecuteCommandLists();
    dx12rhi->PresentAndFlip();
    dx12rhi->FlushCommandQueue();
}


void DX12CommandBuffer::UpdateViewMatrix(Transform *cameraTransform)
{
    Vertex cameraPos = cameraTransform->position;
    XMVECTOR pos = XMVectorSet(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    XMMATRIX view = Math::ViewMatrix(pos, target, up);
    XMStoreFloat4x4(&viewMatrix, view);
}


void DX12CommandBuffer::SetStaticGeometry(Vertex *verts, UINT vertc, UINT *indices, UINT indexc)
{
    if (staticGeo != nullptr)
    {
        delete staticGeo;
    }

    staticGeo = new DX12MeshData(
        device, verts, vertc, indices, indexc
    );

    // static geo is ready. create temporary upload buffers and run upload cmd list
    UploadResourceBuffers(staticGeo->VertexBuffer(), verts, vertc * sizeof(Vertex));
    UploadResourceBuffers(staticGeo->IndexBuffer(), indices, indexc * sizeof(UINT));
}


void DX12CommandBuffer::UploadResourceBuffers(ID3D12Resource *resource, const void *data, UINT dataSizeInBytes)
{
    Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
    ThrowIfFailed(FINFO, device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(dataSizeInBytes),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(uploadBuffer.GetAddressOf())
    ));

    D3D12_SUBRESOURCE_DATA subresourceData = { 0 };
    subresourceData.pData = data;
    subresourceData.RowPitch = dataSizeInBytes;
    subresourceData.SlicePitch = dataSizeInBytes;

    ThrowIfFailed(FINFO, directCommandListAlloc->Reset());
    ThrowIfFailed(FINFO, commandList->Reset(directCommandListAlloc.Get(), nullptr));

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource,
        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
    UpdateSubresources<1>(commandList.Get(), resource, uploadBuffer.Get(), 0, 0, 1, &subresourceData);
    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource,
        D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

    ThrowIfFailed(FINFO, commandList->Close());

    dx12rhi->SubmitCommandList(commandList.Get());
    dx12rhi->ExecuteCommandLists();
    dx12rhi->FlushCommandQueue();
}


void DX12CommandBuffer::BuildDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(FINFO, device->CreateDescriptorHeap(&cbvHeapDesc,
        IID_PPV_ARGS(&cbvHeap)));
}


void DX12CommandBuffer::BuildConstantBuffers()
{
    //objectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, 1, true);

    //UINT objCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

    //D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->Resource()->GetGPUVirtualAddress();
    //// Offset to the ith object constant buffer in the buffer.
    //int boxCBufIndex = 0;
    //cbAddress += boxCBufIndex * objCBByteSize;

    //D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    //cbvDesc.BufferLocation = cbAddress;
    //cbvDesc.SizeInBytes = D3DUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

    //device->CreateConstantBufferView(
    //    &cbvDesc,
    //    cbvHeap->GetCPUDescriptorHandleForHeapStart());
}


void DX12CommandBuffer::BuildRootSignature()
{
    // TODO: implement constant buffer for wvp matrix.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
    rootSigDesc.Init(0, nullptr, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(FINFO, hr);

    ThrowIfFailed(FINFO, device->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)));
}


void DX12CommandBuffer::BuildShadersAndInputLayout()
{
    HRESULT hr = S_OK;

    vertShader = D3DUtil::CompileShader(L"Src/SimpleVS.hlsl", nullptr, "VertMain", "vs_5_0");
    fragShader = D3DUtil::CompileShader(L"Src/SimpleFS.hlsl", nullptr, "FragMain", "ps_5_0");

    inputElementDescs =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
}


void DX12CommandBuffer::BuildInitialGeometry()
{
    // Initialize static geometry with no data.
    //SetStaticGeometry(new Vertex(), 1, new UINT(), 0);
}


void DX12CommandBuffer::BuildPSO()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.InputLayout = { inputElementDescs.data(), (UINT)inputElementDescs.size() };
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.VS =
    {
        reinterpret_cast<BYTE*>(vertShader->GetBufferPointer()),
        vertShader->GetBufferSize()
    };
    psoDesc.PS =
    {
        reinterpret_cast<BYTE*>(fragShader->GetBufferPointer()),
        fragShader->GetBufferSize()
    };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = dx12rhi->BackBufferFormat();
    psoDesc.SampleDesc = dx12rhi->GetSampleDescriptor();
    psoDesc.DSVFormat = dx12rhi->DepthStencilFormat();
    ThrowIfFailed(FINFO, device->CreateGraphicsPipelineState(
        &psoDesc, IID_PPV_ARGS(&pipelineState)
    ));
}
