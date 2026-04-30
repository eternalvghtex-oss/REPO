#pragma once
#include <windows.h>
#include <string>

class Memory {
public:
    bool Attach(DWORD pid);
    void Detach();
    
    template<typename T>
    T Read(uintptr_t addr) {
        T val{};
        SIZE_T read;
        ReadProcessMemory(handle_, (LPCVOID)addr, &val, sizeof(T), &read);
        return val;
    }
    
    bool ReadRaw(uintptr_t addr, void* buf, size_t size);
    uintptr_t GetModuleBase(const char* name);
    
    bool IsAttached() const { return handle_ != nullptr; }
    
private:
    HANDLE handle_ = nullptr;
    DWORD pid_ = 0;
};