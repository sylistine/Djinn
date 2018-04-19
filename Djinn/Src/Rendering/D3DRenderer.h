#pragma once
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "Renderer.h"
class D3DRenderer :
    public Renderer
{
public:
    D3DRenderer(HWND, int, int);
    ~D3DRenderer();
    bool Initialize() override;
    void OnResize() override;
    bool GetMsaa4xState() override;
    void SetMsaa4xState(bool) override;
    void SetClientDimensions(int, int) override;
private:
    static const int swapChainBufferCount = 2;

    HWND hWnd;
    bool msaa4xState = false;
    int width;
    int height;

    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    UINT rtvDescriptorSize = 0;
    UINT dsvDescriptorSize = 0;
    UINT cbvSrvUavDescriptorSize = 0;
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    UINT msaa4xQuality = 0;

    bool CreateCommandObjects();
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> directCommandListAlloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

    bool CreateSwapChain();
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

    bool CreateRtvAndDsvDescriptorHeaps();
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;

    DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    void LogAdapters();
};

