#include "memory.hpp"
#include "utils/utils.hpp"
#include <tlhelp32.h>

bool Memory::Attach(DWORD pid) {
    pid_ = pid;
    handle_ = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    return handle_ != nullptr;
}

void Memory::Detach() {
    if (handle_) {
        CloseHandle(handle_);
        handle_ = nullptr;
    }
}

bool Memory::ReadRaw(uintptr_t addr, void* buf, size_t size) {
    SIZE_T read;
    return ReadProcessMemory(handle_, (LPCVOID)addr, buf, size, &read) && read == size;
}

uintptr_t Memory::GetModuleBase(const char* name) {
    return utils::GetModuleBase(pid_, name);
}