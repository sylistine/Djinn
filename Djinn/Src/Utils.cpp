#include "Utils.h"

DxException::DxException(
    HRESULT errorcode, std::string functionname, std::string filename, int lineNumber) :
    errorcode(errorcode), functionname(functionname), filename(filename), lineNumber(lineNumber)
{}

string DxException::ToString()const
{
    //_com_error error(errorcode);
    return functionname + " failed with error code " + to_string(errorcode) +
        " in " + filename + " on line " + to_string(lineNumber);
}