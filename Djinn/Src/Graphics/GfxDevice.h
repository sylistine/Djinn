#pragma once

#include <wrl.h>

#include <cstdlib>
#include <vector>

#include <dxgi1_4.h>
#include <d3d12.h>
#include "DirectX/DxException.h"


class GfxDevice
{
public:
    GfxDevice(HWND hWnd, int width, int height);
    ~GfxDevice();
    void Initialize();
    HWND HWnd() const;
    void ClientParams(int& width, int& height) const;
    DXGI_FORMAT BackBufferFormat() const;
private:
    HWND hWnd;
    int clientWidth;
    int clientHeight;
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    void LogAdapters();
    void LogAdapterOutputs(IDXGIAdapter* adapter);
    void LogOutputDisplayModes(IDXGIOutput* output, const DXGI_FORMAT format);
};

