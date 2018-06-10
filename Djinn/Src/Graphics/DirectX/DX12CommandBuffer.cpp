#include "DX12CommandBuffer.h"

using namespace Djinn;

DX12CommandBuffer::DX12CommandBuffer(ID3D12Device *device, int clientWidth, int clientHeight)
    : device(device)
    , screenViewport()
    , scissorRect()
{
    // Update viewport and scissor rect.
    screenViewport.TopLeftX = 0;
    screenViewport.TopLeftY = 0;
    screenViewport.Width = static_cast<float>(clientWidth);
    screenViewport.Height = static_cast<float>(clientHeight);
    screenViewport.MinDepth = 0.0f;
    screenViewport.MaxDepth = 1.0f;

    scissorRect = { 0, 0, clientWidth, clientHeight };

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(FINFO, device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));

    ThrowIfFailed(FINFO, device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(directCommandListAlloc.GetAddressOf()))
    );

    ThrowIfFailed(FINFO, device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        directCommandListAlloc.Get(),
        nullptr,
        IID_PPV_ARGS(commandList.GetAddressOf()))
    );

    commandList->Close();
}


DX12CommandBuffer::~DX12CommandBuffer()
{

}

void DX12CommandBuffer::Reset()
{
    ThrowIfFailed(FINFO, directCommandListAlloc->Reset());

    ThrowIfFailed(FINFO, commandList->Reset(directCommandListAlloc.Get(), nullptr));

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    commandList->RSSetViewports(1, &screenViewport);
    commandList->RSSetScissorRects(1, &scissorRect);
}

void DX12CommandBuffer::Clear(float *clearColor)
{
    // Clear to black, baby.
    commandList->ClearRenderTargetView(
        CurrentBackBufferView(),
        clearColor,
        0, nullptr);
    commandList->ClearDepthStencilView(
        DepthStencilView(),
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        1.0f, 0, 0, nullptr);
}


void DX12CommandBuffer::UpdateClientSize(int clientWidth, int clientHeight)
{
    // Update viewport and scissor rect.
    screenViewport.TopLeftX = 0;
    screenViewport.TopLeftY = 0;
    screenViewport.Width = static_cast<float>(clientWidth);
    screenViewport.Height = static_cast<float>(clientHeight);
    screenViewport.MinDepth = 0.0f;
    screenViewport.MaxDepth = 1.0f;

    scissorRect = { 0, 0, clientWidth, clientHeight };
}
