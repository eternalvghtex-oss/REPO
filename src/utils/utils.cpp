#include "utils.hpp"
#include <windows.h>

namespace utils {
    DWORD GetPID(const char* name) {
        DWORD pid = 0;
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 pe{sizeof(pe)};
        
        if (Process32First(snap, &pe)) {
            do {
                if (!_stricmp(pe.szExeFile, name)) {
                    pid = pe.th32ProcessID;
                    break;
                }
            } while (Process32Next(snap, &pe));
        }
        CloseHandle(snap);
        return pid;
    }
    
    uintptr_t GetModuleBase(DWORD pid, const char* modName) {
        uintptr_t base = 0;
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        MODULEENTRY32 me{sizeof(me)};
        
        if (Module32First(snap, &me)) {
            do {
                if (!_stricmp(me.szModule, modName)) {
                    base = (uintptr_t)me.modBaseAddr;
                    break;
                }
            } while (Module32Next(snap, &me));
        }
        CloseHandle(snap);
        return base;
    }
    
    HWND FindGameWindow() {
        HWND hwnd = nullptr;
        EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
            char title[256];
            GetWindowTextA(hWnd, title, sizeof(title));
            if (strstr(title, "Counter-Strike 2") || strstr(title, "CS2")) {
                *(HWND*)lParam = hWnd;
                return FALSE;
            }
            return TRUE;
        }, (LPARAM)&hwnd);
        return hwnd;
    }
}