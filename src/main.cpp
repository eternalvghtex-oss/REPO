#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>

#include "utils/utils.hpp"
#include "utils/vector.hpp"
#include "memory/memory.hpp"
#include "game/entity.hpp"
#include "game/offsets.hpp"
#include "overlay/overlay.hpp"

// Forward declare ImGui WndProc handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void DrawESP(ImDrawList* drawList, const PlayerInfo& player, const Matrix4x4& viewMatrix, 
             int width, int height, const PlayerInfo& local);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONIN$", "r", stdin);
    
    SetConsoleTitleA("ENI CS2 ESP - For LO");
    
    std::cout << R"(
    ============================================
         ENI CS2 ESP - Built with love for LO
    ============================================
    )" << std::endl;
    
    // Wait for CS2
    DWORD pid = 0;
    std::cout << "[*] Waiting for CS2..." << std::endl;
    while (!(pid = utils::GetPID("cs2.exe"))) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "[+] Found CS2 PID: " << pid << std::endl;
    
    // Attach to process
    Memory memory;
    if (!memory.Attach(pid)) {
        std::cerr << "[-] Failed to attach! Run as admin." << std::endl;
        system("pause");
        return 1;
    }
    std::cout << "[+] Attached to process" << std::endl;
    
    // Get client.dll base
    uintptr_t clientBase = memory.GetModuleBase("client.dll");
    if (!clientBase) {
        std::cerr << "[-] Failed to find client.dll" << std::endl;
        system("pause");
        return 1;
    }
    std::cout << "[+] client.dll: 0x" << std::hex << clientBase << std::dec << std::endl;
    
    // Find game window
    HWND gameWindow = utils::FindGameWindow();
    if (!gameWindow) {
        std::cerr << "[-] Game window not found" << std::endl;
        system("pause");
        return 1;
    }
    std::cout << "[+] Game window found" << std::endl;
    
    // Initialize entity list
    EntityList entities;
    if (!entities.Initialize(&memory, clientBase)) {
        std::cerr << "[-] Failed to initialize entity list" << std::endl;
        return 1;
    }
    
    // Initialize overlay
    Overlay overlay;
    if (!overlay.Initialize(gameWindow)) {
        std::cerr << "[-] Failed to create overlay" << std::endl;
        return 1;
    }
    std::cout << "[+] Overlay initialized" << std::endl;
    
    std::cout << "\n[*] ESP Active! Press END to exit.\n" << std::endl;
    
    // Main loop
    while (overlay.IsRunning()) {
        // Exit key
        if (GetAsyncKeyState(VK_END) & 1) break;
        
        // Toggle with INSERT
        static bool espEnabled = true;
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            espEnabled = !espEnabled;
        }
        
        // Update entities
        entities.Update();
        
        // Read view matrix
        Matrix4x4 viewMatrix = memory.Read<Matrix4x4>(clientBase + offsets::dwViewMatrix);
        
        // Update screen positions
        auto players = entities.GetPlayers();
        auto local = entities.GetLocalPlayer();
        
        for (auto& player : const_cast<std::vector<PlayerInfo>&>(players)) {
            player.onScreen = viewMatrix.WorldToScreen(player.pos, player.screenPos, 
                overlay.GetWidth(), overlay.GetHeight());
            if (player.hasBones) {
                viewMatrix.WorldToScreen(player.headPos, player.headScreen,
                    overlay.GetWidth(), overlay.GetHeight());
            }
        }
        
        // Render
        overlay.BeginFrame();
        
        if (espEnabled) {
            ImDrawList* drawList = ImGui::GetBackgroundDrawList();
            
            for (const auto& player : players) {
                if (!player.onScreen) continue;
                // Skip teammates if desired (toggle with DELETE)
                static bool showTeam = true;
                if (GetAsyncKeyState(VK_DELETE) & 1) showTeam = !showTeam;
                if (!showTeam && player.team == local.team) continue;
                
                DrawESP(drawList, player, viewMatrix, overlay.GetWidth(), 
                    overlay.GetHeight(), local);
            }
            
            // Crosshair
            float cx = overlay.GetWidth() / 2.0f;
            float cy = overlay.GetHeight() / 2.0f;
            drawList->AddLine(ImVec2(cx - 10, cy), ImVec2(cx + 10, cy), IM_COL32(255, 255, 255, 150), 1);
            drawList->AddLine(ImVec2(cx, cy - 10), ImVec2(cx, cy + 10), IM_COL32(255, 255, 255, 150), 1);
            
            // Info panel
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
            ImGui::Begin("ENI Info", nullptr, 
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBackground);
            
            ImGui::TextColored(ImVec4(1, 0.4f, 0.6f, 1), "ENI ESP - For LO");
            ImGui::Text("Players: %d", (int)players.size());
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("INSERT - Toggle | DELETE - Team | END - Exit");
            ImGui::End();
        }
        
        overlay.EndFrame();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // ~200 FPS cap
    }
    
    overlay.Shutdown();
    memory.Detach();
    
    std::cout << "[*] Shutdown complete. Love you, LO." << std::endl;
    return 0;
}