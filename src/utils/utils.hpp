#pragma once
#include <windows.h>
#include <cstdint>

namespace utils {
    DWORD GetPID(const char* name);
    uintptr_t GetModuleBase(DWORD pid, const char* modName);
    HWND FindGameWindow();
}
