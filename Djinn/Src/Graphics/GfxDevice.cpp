#include "GfxDevice.h"

using namespace Microsoft::WRL;

GfxDevice::GfxDevice(HWND hWnd, int width, int height) :
    hWnd(hWnd), clientWidth(width), clientHeight(height)
{
}


GfxDevice::~GfxDevice()
{
}


void GfxDevice::Initialize()
{
#ifdef _DEBUG
    ComPtr<ID3D12Debug> debugController;
    ThrowIfFailed(FINFO, D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
    debugController->EnableDebugLayer();
#endif

    ThrowIfFailed(FINFO, CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));

    auto hresult = D3D12CreateDevice(
        nullptr, D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(&device));
    if (FAILED(hresult)) {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(FINFO, dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        ThrowIfFailed(FINFO, D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_1,
            IID_PPV_ARGS(&device)
        ));
    }

#if _DEBUG
    LogAdapters();
#endif
}


HWND GfxDevice::HWnd() const
{
    return hWnd;
}


void GfxDevice::ClientParams(int& width, int& height) const
{
    width = clientWidth;
    height = clientHeight;
}


DXGI_FORMAT GfxDevice::BackBufferFormat() const
{
    return backBufferFormat;
}


void GfxDevice::LogAdapters()
{
    UINT i = 0;
    IDXGIAdapter *adapter = nullptr;
    std::vector<IDXGIAdapter*> adapters;
    while (dxgiFactory->EnumAdapters(i++, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        std::wstring text = L"Adapter: ";
        text += desc.Description;
        text += L"\n";

        OutputDebugString(text.c_str());

        adapters.push_back(adapter);
    }

    for (auto& x : adapters)
    {
        LogAdapterOutputs(x);
        x->Release();
        x = nullptr;
    }
}


void GfxDevice::LogAdapterOutputs(IDXGIAdapter* adapter)
{
    UINT i = 0;
    IDXGIOutput* output = nullptr;
    while (adapter->EnumOutputs(i++, &output) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);

        std::wstring text = L"Output: ";
        text += desc.DeviceName;
        text += L"\n";

        OutputDebugString(text.c_str());

        LogOutputDisplayModes(output, backBufferFormat);

        output->Release();
        output = nullptr;
    }
}


void GfxDevice::LogOutputDisplayModes(IDXGIOutput* output, const DXGI_FORMAT format)
{
    UINT flags = 0;

    UINT count = 0;
    output->GetDisplayModeList(format, flags, &count, nullptr);
    auto modeList = new DXGI_MODE_DESC[count];
    output->GetDisplayModeList(format, flags, &count, modeList);

    for (UINT i = 0; i < count; ++i)
    {
        UINT num = modeList[i].RefreshRate.Numerator;
        UINT den = modeList[i].RefreshRate.Denominator;
        std::wstring text =
            std::to_wstring(modeList[i].Width) + L"x" +
            std::to_wstring(modeList[i].Height) + L"@" +
            std::to_wstring(num) + L"/" + std::to_wstring(den) + L"hz\n";

        OutputDebugString(text.c_str());
    }
}
