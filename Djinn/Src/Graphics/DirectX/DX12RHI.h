#pragma once
#include <wrl.h>
#include <cstdlib>

#include <vector>
#include <string>

#include <dxgi1_4.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "d3dx12.h"

#include "../GfxRHI.h"

#include "DX12CommandBuffer.h"


#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3D12.lib")


namespace Djinn {
    class DX12CommandBuffer;

    typedef struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 color;
    } vertex;

    class DX12RHI : public GfxRHI
    {
    public:
        // ~tors
        DX12RHI(HWND hWnd, int width, int height);
        ~DX12RHI() override;

        MSAA_SAMPLE_LEVEL GetMsaaSampleLevel()override;
        bool IsInitialized()override;
        void SetMsaaSampleLevel(MSAA_SAMPLE_LEVEL newLevel)override;
        void SetClientDimensions(int width, int height)override;
        ID3D12Resource * CurrentBackBuffer()const;
        D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
        D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
        DXGI_SAMPLE_DESC GetSampleDescriptor()const;
        DXGI_FORMAT BackBufferFormat()const;
        DXGI_FORMAT DepthStencilFormat()const;

        bool Initialize() override;
        CommandBuffer *GetCommandBuffer() override;
        void SubmitCommandList(ID3D12CommandList *commandList);
        void ExecuteCommandLists();
        void PresentAndFlip();
        void FlushCommandQueue();
    private:
        DX12RHI(const DX12RHI& other);

        HWND hWnd;
        int clientWidth;
        int clientHeight;
        bool initialized = false;
        Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
        Microsoft::WRL::ComPtr<ID3D12Device> device;
        Microsoft::WRL::ComPtr<ID3D12Fence> fence;
        UINT64 currentFence = 0;
        UINT msaa4xMaxQuality = 0;
        UINT msaa8xMaxQuality = 0;
        MSAA_SAMPLE_LEVEL msaaSampleLevel = MSAA_SAMPLE_LEVEL_1X;

        static const int swapChainBufferCount = 2;
        DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        int currentBackBuffer = 0;
        Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
        Microsoft::WRL::ComPtr<ID3D12Resource> swapChainBuffer[swapChainBufferCount];

        Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> directCommandAllocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

        UINT rtDescriptorSize = 0;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
        UINT dsDescriptorSize = 0;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
        DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilTexture;
        UINT cbvSrvUavDescriptorSize = 0;

        D3D12_VIEWPORT screenViewport;
        D3D12_RECT scissorRect;

        // TODO: This shouldn't live here... we need a controller to hold these...
        std::vector<ID3D12CommandList *> commandLists;

        void OnResize();
        void UpdateMSAASupport();
        bool CreateCommandObjects();
        void CreateSwapChain();
        void CreateRtvAndDsvDescriptorHeaps();


        void LogAdapters();
        void LogAdapterOutputs(IDXGIAdapter* adapter);
        void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
    };
}
