#pragma once
#include <vector>
#include "utils/vector.hpp"
#include "memory/memory.hpp"
#include "offsets.hpp"

struct PlayerInfo {
    Vec3 pos;
    Vec3 headPos;
    int health;
    int team;
    int lifeState;
    bool isLocal;
    bool isAlive;
    bool isScoped;
    int shotsFired;
    float distance;
    Vec2 screenPos;
    Vec2 headScreen;
    bool onScreen;
    Vec3 bonePositions[19]; // для скелета
    bool hasBones;
};

class EntityList {
public:
    bool Initialize(Memory* mem, uintptr_t clientBase);
    void Update();
    const std::vector<PlayerInfo>& GetPlayers() const { return players_; }
    PlayerInfo GetLocalPlayer() const { return local_; }
    
private:
    Memory* mem_ = nullptr;
    uintptr_t clientBase_ = 0;
    std::vector<PlayerInfo> players_;
    PlayerInfo local_{};
    
    bool ReadPlayer(uintptr_t controller, PlayerInfo& out, bool isLocal);
    bool ReadBones(uintptr_t pawn, PlayerInfo& out);
    Vec3 GetBonePos(uintptr_t boneArray, int boneId);
};
