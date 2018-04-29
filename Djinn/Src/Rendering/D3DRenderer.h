#pragma once
#include <wrl.h>
#include <cstdlib>

#include <vector>
#include <string>

#include <dxgi1_4.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include "Renderer.h"
#include "../Utils.h"


#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3D12.lib")


using namespace Microsoft::WRL;


class D3DRenderer : public Renderer
{
public:
    // ~tors
    D3DRenderer(HWND, int, int);
    ~D3DRenderer();

public:
    bool GetMsaa4xState() override;
    void SetMsaa4xState(bool) override;
    void SetClientDimensions(int width, int height) override;
private:
    ID3D12Resource *CurrentBackBuffer()const;
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

private:
    HWND hWnd;
    int clientWidth;
    int clientHeight;

    ComPtr<IDXGIFactory4> dxgiFactory;
    ComPtr<ID3D12Device> device;
    ComPtr<ID3D12Fence> fence;
    UINT64 currentFence = 0;
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    bool msaa4xState = false;
    UINT msaa4xQuality = 0;

    ComPtr<ID3D12CommandQueue> commandQueue;
    ComPtr<ID3D12CommandAllocator> directCommandListAlloc;
    ComPtr<ID3D12GraphicsCommandList> commandList;

    ComPtr<IDXGISwapChain> swapChain;
    static const int swapChainBufferCount = 2;
    int currentBackBuffer;
    ComPtr<ID3D12Resource> swapChainBuffer[swapChainBufferCount];

    UINT rtvDescriptorSize = 0;
    ComPtr<ID3D12DescriptorHeap> rtvHeap;
    UINT dsvDescriptorSize = 0;
    ComPtr<ID3D12DescriptorHeap> dsvHeap;
    DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    ComPtr<ID3D12Resource> depthStencilBuffer;
    UINT cbvSrvUavDescriptorSize = 0;

    D3D12_VIEWPORT screenViewport;
    D3D12_RECT scissorRect;

public:
    bool Initialize() override;
    void Draw() override;
private:
    void OnResize();
    void FlushCommandQueue();
    bool CreateCommandObjects();
    void CreateSwapChain();
    void CreateRtvAndDsvDescriptorHeaps();


    void LogAdapters();
    void LogAdapterOutputs(IDXGIAdapter* adapter);
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
};
