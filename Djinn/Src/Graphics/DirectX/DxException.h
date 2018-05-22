#pragma once

#include "../../stdinc.h"

// credit to Frank Luna for the particular structure of these utilities.
class DxException : std::exception
{
public:
    DxException(
        HRESULT errorcode,
        const std::wstring& function,
        const std::wstring& file,
        int line);
    std::wstring ToString()const;
private:
    HRESULT errorCode;
    std::wstring errorMsg;
    std::wstring function;
    std::wstring file;
    int line;
};

#define FINFO __FILEW__, __FUNCTIONW__, __LINE__

inline void ThrowIfFailed(
    const std::wstring& filename,
    const std::wstring& functionname,
    const int linenumber,
    const HRESULT result)
{
    if (FAILED(result))
    {
        DxException e(result, functionname, filename, linenumber);
        OutputDebugString(e.ToString().c_str());
        throw e;
    }
}
