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
    
    // Read local controller
    uintptr_t localController = mem_->Read<uintptr_t>(clientBase_ + offsets::dwLocalPlayerController);
    if (!localController) return;
    
    // Read local pawn for position
    uintptr_t localPawn = mem_->Read<uintptr_t>(clientBase_ + offsets::dwLocalPlayerPawn);
    
    // Read local data
    local_.isLocal = true;
    local_.health = mem_->Read<int>(localController + offsets::m_iHealth);
    local_.team = mem_->Read<int>(localController + offsets::m_iTeamNum);
    local_.lifeState = mem_->Read<int>(localController + offsets::m_lifeState);
    local_.isAlive = local_.lifeState == 0 && local_.health > 0;
    
    if (localPawn) {
        uintptr_t gameScene = mem_->Read<uintptr_t>(localPawn + offsets::m_pGameSceneNode);
        if (gameScene) {
            local_.pos = mem_->Read<Vec3>(gameScene + offsets::m_vecOrigin);
        }
    }
    
    // Read entity list
    uintptr_t entityList = mem_->Read<uintptr_t>(clientBase_ + offsets::dwEntityList);
    if (!entityList) return;
    
    // First entry
    uintptr_t listEntry = mem_->Read<uintptr_t>(entityList + 0x10);
    if (!listEntry) return;
    
    // Iterate up to 64 players
    for (int i = 0; i < 64; i++) {
        uintptr_t controller = mem_->Read<uintptr_t>(listEntry + i * 0x78);
        if (!controller || controller == localController) continue;
        
        PlayerInfo player;
        if (ReadPlayer(controller, player, false)) {
            player.distance = local_.pos.DistTo(player.pos);
            players_.push_back(player);
        }
    }
    
    // Sort by distance (closest first)
    std::sort(players_.begin(), players_.end(), 
        [](const PlayerInfo& a, const PlayerInfo& b) {
            return a.distance < b.distance;
        });
}

bool EntityList::ReadPlayer(uintptr_t controller, PlayerInfo& out, bool isLocal) {
    out = {};
    out.isLocal = isLocal;
    
    // Basic data
    out.health = mem_->Read<int>(controller + offsets::m_iHealth);
    out.team = mem_->Read<int>(controller + offsets::m_iTeamNum);
    out.lifeState = mem_->Read<int>(controller + offsets::m_lifeState);
    out.isAlive = out.lifeState == 0 && out.health > 0 && out.health <= 100;
    
    if (!out.isAlive) return false;
    
    // Get pawn from handle
    uint32_t pawnHandle = mem_->Read<uint32_t>(controller + offsets::m_hPlayerPawn);
    if (!pawnHandle) return false;
    
    uint32_t pawnIndex = (pawnHandle & 0x7FFF);
    uint32_t listIndex = pawnIndex / 0x20;
    uint32_t listOffset = pawnIndex % 0x20;
    
    // Read entity list for pawn
    uintptr_t entityList = mem_->Read<uintptr_t>(clientBase_ + offsets::dwEntityList);
    uintptr_t pawnList = mem_->Read<uintptr_t>(entityList + 0x10 + listIndex * 8);
    if (!pawnList) return false;
    
    uintptr_t pawn = mem_->Read<uintptr_t>(pawnList + listOffset * 0x78);
    if (!pawn) return false;
    
    // Position
    // Position from game scene node (CGameSceneNode::m_vecOrigin)
    uintptr_t gameScene = mem_->Read<uintptr_t>(pawn + offsets::m_pGameSceneNode);
    if (!gameScene) return false;
    out.pos = mem_->Read<Vec3>(gameScene + offsets::m_vecOrigin);
    
    // Calculate head position (approximate)
    out.headPos = out.pos;
    out.headPos.z += 72.0f; // примерная высота игрока
    
    // Read bones for skeleton ESP
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
