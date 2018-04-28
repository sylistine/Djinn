#include "Utils.h"
#include <comdef.h>

DxException::DxException(
    HRESULT errorCode, const wstring& function, const wstring& file, int line) :
    errorCode(errorCode), function(function), file(file), line(line)
{
    errorMsg = _com_error(errorCode).ErrorMessage();
}

wstring DxException::ToString()const
{
    return L"DxException: " + errorMsg + L" (" + to_wstring(errorCode) + L")\n" +
        function + L" : " + file + L" (" + to_wstring(line) + L")\n\n";
}