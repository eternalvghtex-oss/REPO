#include "entity.hpp"
#include <algorithm>

bool EntityList::Initialize(Memory* mem, uintptr_t clientBase) {
    mem_ = mem;
    clientBase_ = clientBase;
    return mem_->IsAttached();
}

void EntityList::Update() {
    players_.clear();
    if (!mem_->IsAttached()) return;

    const uintptr_t localController = mem_->Read<uintptr_t>(clientBase_ + offsets::dwLocalPlayerController);
    if (!localController) return;

    const uintptr_t localPawn = mem_->Read<uintptr_t>(clientBase_ + offsets::dwLocalPlayerPawn);
    local_ = {};
    if (localPawn) {
        ReadPlayer(localController, localPawn, local_, true);
    } else {
        local_.isLocal = true;
    }

    // Controller indices for players are typically 1..64.
    for (uint32_t i = 1; i <= 64; ++i) {
        const uintptr_t controller = ResolveEntityFromIndex(i);
        if (!controller || controller == localController) continue;

        const uint32_t pawnHandle = mem_->Read<uint32_t>(controller + offsets::m_hPlayerPawn);
        const uintptr_t pawn = ResolvePawnFromHandle(pawnHandle);
        if (!pawn) continue;

        PlayerInfo player{};
        if (!ReadPlayer(controller, pawn, player, false)) continue;
        player.distance = local_.pos.DistTo(player.pos);
        players_.push_back(player);
    }

    std::sort(players_.begin(), players_.end(), 
        [](const PlayerInfo& a, const PlayerInfo& b) {
            return a.distance < b.distance;
        });
}

uintptr_t EntityList::ResolveEntityFromIndex(uint32_t index) const {
    const uintptr_t entityList = mem_->Read<uintptr_t>(clientBase_ + offsets::dwEntityList);
    if (!entityList) return 0;

    const uintptr_t listEntry = mem_->Read<uintptr_t>(entityList + 0x10 + 0x8 * ((index & 0x7FFF) >> 9));
    if (!listEntry) return 0;

    return mem_->Read<uintptr_t>(listEntry + 0x78 * (index & 0x1FF));
}

uintptr_t EntityList::ResolvePawnFromHandle(uint32_t pawnHandle) const {
    if (!pawnHandle) return 0;
    return ResolveEntityFromIndex(pawnHandle & 0x7FFF);
}

bool EntityList::ReadPlayer(uintptr_t controller, uintptr_t pawn, PlayerInfo& out, bool isLocal) {
    out = {};
    out.isLocal = isLocal;

    // Read from pawn (controller doesn't own these fields).
    out.health = mem_->Read<int>(pawn + offsets::m_iHealth);
    out.team = mem_->Read<int>(pawn + offsets::m_iTeamNum);
    out.lifeState = mem_->Read<int>(pawn + offsets::m_lifeState);
    out.shotsFired = mem_->Read<int>(pawn + offsets::m_iShotsFired);
    out.isAlive = out.lifeState == 0 && out.health > 0 && out.health <= 100;
    if (!out.isAlive) return false;

    const uintptr_t gameScene = mem_->Read<uintptr_t>(pawn + offsets::m_pGameSceneNode);
    if (!gameScene) return false;
    out.pos = mem_->Read<Vec3>(gameScene + offsets::m_vecOrigin);

    out.headPos = out.pos;
    out.headPos.z += 72.0f;

    out.hasBones = ReadBones(pawn, out);
    return true;
}

bool EntityList::ReadBones(uintptr_t pawn, PlayerInfo& out) {
    uintptr_t gameScene = mem_->Read<uintptr_t>(pawn + offsets::m_pGameSceneNode);
    if (!gameScene) return false;
    
    uintptr_t boneArray = mem_->Read<uintptr_t>(gameScene + offsets::m_modelState + offsets::m_pBoneArray);
    if (!boneArray) return false;
    
    // Common bone IDs for CS2
    int boneIds[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
    for (int i = 0; i < 19; i++) {
        out.bonePositions[i] = GetBonePos(boneArray, boneIds[i]);
    }
    
    // Update head position from actual head bone (usually bone 6)
    if (19 > 6) {
        out.headPos = out.bonePositions[6];
    }
    
    return true;
}

Vec3 EntityList::GetBonePos(uintptr_t boneArray, int boneId) {
    // Each bone is 32 bytes (Vec3 pos + padding + Vec3 scale + padding)
    return mem_->Read<Vec3>(boneArray + boneId * 32);
}
