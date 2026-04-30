#pragma once
#include <cstdint>

namespace offsets {
    // Source: a2x/cs2-dumper (user-provided snapshot date 2026-04-29)
    constexpr uintptr_t dwEntityList = 0x24CEC60;
    constexpr uintptr_t dwLocalPlayerController = 0x23084C0;
    constexpr uintptr_t dwLocalPlayerPawn = 0x20546A0;
    constexpr uintptr_t dwViewMatrix = 0x232E9C0;
    constexpr uintptr_t dwGameEntitySystem = 0x24CEC60;
    constexpr uintptr_t dwGlobalVars = 0x20495A0;

    // Client classes / netvars
    constexpr uintptr_t m_vecOrigin = 0x88;      // CGameSceneNode::m_vecOrigin
    constexpr uintptr_t m_iHealth = 0x344;
    constexpr uintptr_t m_iTeamNum = 0x3E3;
    constexpr uintptr_t m_lifeState = 0x348;
    constexpr uintptr_t m_pGameSceneNode = 0x328;
    constexpr uintptr_t m_modelState = 0x170;
    // Controller -> pawn handle (dwPlayerPawn / m_hPlayerPawn)
    constexpr uintptr_t dwPlayerPawn = 0x7BC;
    constexpr uintptr_t m_hPlayerPawn = dwPlayerPawn;
    constexpr uintptr_t m_iShotsFired = 0x22B0;
    constexpr uintptr_t m_pBoneArray = 0x80;     // CModelState + m_boneMatrix
}
