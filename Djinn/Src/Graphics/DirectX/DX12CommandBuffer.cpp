#include "DX12CommandBuffer.h"

#include "../../Color.h"
#include "../../Timer.h"

using namespace Djinn;
using namespace DirectX;

DX12CommandBuffer::DX12CommandBuffer(DX12RHI *dx12rhi, ID3D12Device *device)
    : dx12rhi(dx12rhi)
    , device(device)
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
    BuildBoxGeometry();
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

    ThrowIfFailed(FINFO, commandList->Reset(directCommandListAlloc.Get(), nullptr));

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        dx12rhi->CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    ));

    commandList->RSSetViewports(1, &screenViewport);
    commandList->RSSetScissorRects(1, &scissorRect);
    // Clear to black, baby.
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


void DX12CommandBuffer::BuildDescriptorHeaps()
{

}


void DX12CommandBuffer::BuildConstantBuffers()
{

}


void DX12CommandBuffer::BuildRootSignature()
{

}


void DX12CommandBuffer::BuildShadersAndInputLayout()
{

}


void DX12CommandBuffer::BuildBoxGeometry()
{

}


void DX12CommandBuffer::BuildPSO()
{

}
