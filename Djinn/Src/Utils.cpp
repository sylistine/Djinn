#include "Utils.h"

DxException::DxException(
    HRESULT errorcode, const wstring& function, const wstring& file, int line) :
    errorcode(errorcode), function(function), file(file), line(line)
{}

wstring DxException::ToString()const
{
    //_com_error error(errorcode);
    return function + L" failed with error code " + to_wstring(errorcode) +
        L" in " + file + L" on line " + to_wstring(line);
}