#pragma once
#include <windows.h>

namespace utils {
    DWORD GetPID(const char* name);
    uintptr_t GetModuleBase(DWORD pid, const char* modName);
    HWND FindGameWindow();
}