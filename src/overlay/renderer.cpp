#include "overlay.hpp"
#include "../game/entity.hpp"
#include "../utils/vector.hpp"

void DrawESP(ImDrawList* drawList, const PlayerInfo& player, const Matrix4x4& viewMatrix, 
             int width, int height, const PlayerInfo& local) {
    if (!player.onScreen) return;
    
    // Colors
    ImU32 color;
    if (player.team == local.team) {
        color = IM_COL32(0, 255, 100, 255); // Teammate - green
    } else {
        color = IM_COL32(255, 50, 50, 255);  // Enemy - red
    }
    
    if (player.health <= 30) {
        color = IM_COL32(255, 200, 0, 255); // Low HP - yellow
    }
    
    float boxHeight = player.screenPos.y - player.headScreen.y;
    float boxWidth = boxHeight * 0.5f;
    float boxX = player.headScreen.x - boxWidth / 2;
    float boxY = player.headScreen.y;
    
    // Box
    drawList->AddRect(
        ImVec2(boxX, boxY),
        ImVec2(boxX + boxWidth, boxY + boxHeight),
        color, 0, 0, 1.5f
    );
    
    // Health bar background
    float barWidth = 4.0f;
    float barHeight = boxHeight;
    float barX = boxX - barWidth - 2;
    float barY = boxY;
    
    drawList->AddRectFilled(
        ImVec2(barX, barY),
        ImVec2(barX + barWidth, barY + barHeight),
        IM_COL32(0, 0, 0, 180)
    );
    
    // Health bar
    float healthHeight = barHeight * (player.health / 100.0f);
    ImU32 healthColor = player.health > 60 ? IM_COL32(0, 255, 0, 255) :
                        player.health > 30 ? IM_COL32(255, 255, 0, 255) :
                        IM_COL32(255, 0, 0, 255);
    
    drawList->AddRectFilled(
        ImVec2(barX, barY + (barHeight - healthHeight)),
        ImVec2(barX + barWidth, barY + barHeight),
        healthColor
    );
    
    // Snapline to bottom center
    drawList->AddLine(
        ImVec2(width / 2.0f, height),
        ImVec2(player.screenPos.x, player.screenPos.y),
        color, 1.0f
    );
    
    // Info text
    char info[128];
    sprintf_s(info, "%d HP | %.1fm", player.health, player.distance);
    
    ImVec2 textSize = ImGui::CalcTextSize(info);
    drawList->AddRectFilled(
        ImVec2(boxX, boxY - textSize.y - 2),
        ImVec2(boxX + textSize.x + 4, boxY),
        IM_COL32(0, 0, 0, 180)
    );
    
    drawList->AddText(
        ImVec2(boxX + 2, boxY - textSize.y),
        IM_COL32(255, 255, 255, 255),
        info
    );
    
    // Skeleton ESP
    if (player.hasBones) {
        ImU32 boneColor = IM_COL32(255, 255, 255, 200);
        Vec2 boneScreen[19];
        bool boneVisible[19] = {false};
        
        for (int i = 0; i < 19; i++) {
            boneVisible[i] = viewMatrix.WorldToScreen(player.bonePositions[i], boneScreen[i], width, height);
        }
        
        // Draw skeleton connections
        int connections[][2] = {
            {0, 1}, {1, 2}, {2, 3}, // spine
            {3, 4}, {4, 5}, // neck to head
            {2, 6}, {6, 7}, {7, 8}, // left arm
            {2, 9}, {9, 10}, {10, 11}, // right arm
            {0, 12}, {12, 13}, {13, 14}, // left leg
            {0, 15}, {15, 16}, {16, 17}, // right leg
        };
        
        for (auto& conn : connections) {
            if (boneVisible[conn[0]] && boneVisible[conn[1]]) {
                drawList->AddLine(
                    ImVec2(boneScreen[conn[0]].x, boneScreen[conn[0]].y),
                    ImVec2(boneScreen[conn[1]].x, boneScreen[conn[1]].y),
                    boneColor, 1.0f
                );
            }
        }
    }
}