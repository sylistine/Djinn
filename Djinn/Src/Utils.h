#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <string>
#include <memory>
#include <vector>

using namespace std;

// credit to Frank Luna for the particular structure of these utilities.
class DxException : exception
{
public:
    DxException(HRESULT errorcode, const wstring& function, const wstring& file, int line);
    wstring ToString()const;
private:
    HRESULT errorCode;
    wstring errorMsg;
    wstring function;
    wstring file;
    int line;
};

inline wstring AnsiToWString(const string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return wstring(buffer);
}

#define ThrowIfFailed(x) {                                       \
    HRESULT errorcode = (x);                                     \
    wstring filename = AnsiToWString(__FILE__);                  \
    DxException e(errorcode, L#x, filename, __LINE__);           \
    OutputDebugString(e.ToString().c_str());                     \
    if (FAILED(errorcode)) {                                     \
        throw e;                                                 \
    }                                                            \
}
