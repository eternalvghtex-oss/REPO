#pragma once
#include <cstdint>

// CS2 offsets - ОБНОВЛЯТЬ ПОД КАЖДЫЙ ПАТЧ!
// Используй: https://github.com/a2x/cs2-dumper или hazedumper
namespace offsets {
    // Client.dll
    constexpr uintptr_t dwEntityList = 0x1A1F698;      // 2026-04-30
    constexpr uintptr_t dwLocalPlayerController = 0x1A69DA0;
    constexpr uintptr_t dwViewMatrix = 0x1A7F910;
    constexpr uintptr_t dwLocalPlayerPawn = 0x1834E10;
    constexpr uintptr_t dwGameTypes = 0x1A22080;
    constexpr uintptr_t dwGameRules = 0x1A67F08;
    
    // Entity
    constexpr uintptr_t m_iHealth = 0x344;
    constexpr uintptr_t m_iTeamNum = 0x3E3;
    constexpr uintptr_t m_vecOrigin = 0x38;
    constexpr uintptr_t m_hPlayerPawn = 0x80C;
    constexpr uintptr_t m_lifeState = 0x348;
    constexpr uintptr_t m_iCompTeammateColor = 0x750;
    constexpr uintptr_t m_angEyeAngles = 0x1578;
    constexpr uintptr_t m_iShotsFired = 0x22B0;
    constexpr uintptr_t m_bIsScoped = 0x22A0;
    
    // Pawn
    constexpr uintptr_t m_vOldOrigin = 0x127C;
    constexpr uintptr_t m_pGameSceneNode = 0x328;
    constexpr uintptr_t m_modelState = 0x170;
    constexpr uintptr_t m_vecAbsOrigin = 0xD0;
    
    // Bone matrix (для скелетного ESP)
    constexpr uintptr_t m_pBoneArray = 0x80;
}

// Чтобы обновить offsets автоматически:
// 1. Скачай cs2-dumper: git clone https://github.com/a2x/cs2-dumper
// 2. Запусти после обновления CS2
// 3. Скопируй значения сюда