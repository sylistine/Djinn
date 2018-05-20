#include "DxException.h"

#include <comdef.h>

DxException::DxException(
    HRESULT errorCode, const std::wstring& function, const std::wstring& file, int line) :
    errorCode(errorCode), function(function), file(file), line(line)
{
    errorMsg = _com_error(errorCode).ErrorMessage();
}

std::wstring DxException::ToString()const
{
    return L"DxException: " + errorMsg + L" (" + std::to_wstring(errorCode) + L")\n" +
        function + L" : " + file + L" (" + std::to_wstring(line) + L")\n\n";
}
