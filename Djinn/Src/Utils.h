#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>

using namespace std;

class DxException : exception
{
public:
    DxException(HRESULT errorcode, std::string functionname, std::string filename, int lineNumber);
    string ToString()const;
private:
    HRESULT errorcode;
    string functionname;
    string filename;
    int lineNumber;
};

#define ThrowIfFailed(x)                                                             \
{                                                                                    \
    HRESULT errorcode = (x);                                                         \
    std::string filename = __FILE__;                                                      \
    if (FAILED(errorcode)) { throw DxException(errorcode, #x, filename, __LINE__); }\
}
