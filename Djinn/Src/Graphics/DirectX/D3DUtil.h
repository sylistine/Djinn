#pragma once

#include <wrl.h>
#include <string>

#include <d3d12.h>
#include <d3dcompiler.h>

#include "DxException.h"

namespace Djinn
{
    class D3DUtil
    {
    public:
        static UINT CalcConstantBufferByteSize(UINT sizeInBytes);
        static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
            const std::wstring& filename,
            const D3D_SHADER_MACRO* defines,
            const std::string& entrypoint,
            const std::string& target);
    };
}