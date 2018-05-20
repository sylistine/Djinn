#pragma once

#include <Windows.h>

#include <string>


inline std::wstring MultiByteToWideString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}
