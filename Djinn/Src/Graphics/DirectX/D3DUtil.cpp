#include "D3DUtil.h"

using namespace Djinn;
using namespace Microsoft::WRL;

/// Kept as a function to keep it self-documenting.
UINT D3DUtil::CalcConstantBufferByteSize(UINT sizeInBytes)
{
    return (sizeInBytes + 255) & ~255;
}

Microsoft::WRL::ComPtr<ID3DBlob> D3DUtil::CompileShader(
    const std::wstring& filename,
    const D3D_SHADER_MACRO* defines,
    const std::string& entrypoint,
    const std::string& target)
{
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = S_OK;

    ComPtr<ID3DBlob> byteCode = nullptr;
    ComPtr<ID3DBlob> errors;
    hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

    if (errors != nullptr)
        OutputDebugStringA((char*)errors->GetBufferPointer());

    ThrowIfFailed(FINFO, hr);

    return byteCode;
}