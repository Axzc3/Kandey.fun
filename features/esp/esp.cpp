#include "esp.h"
#include "../../core/sdk/cpp/offsets.hpp"
#include "imgui.h"
#include <d3d11.h>
#include <Windows.h>

namespace esp {
    bool enabled = false;
    ImColor boxColor = ImColor(255, 0, 0, 255); // Red
    float boxThickness = 2.0f;
    int lastPlayerCount = 0;

    // Simple memory read
    template<typename T>
    T Read(uintptr_t address) {
        if (!address) return T{};
        __try {
            return *(T*)address;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            return T{};
        }
    }

    struct Vector3 { float x, y, z; };
    struct Vector2 { float x, y; };
    struct ViewMatrix { float m[4][4]; };

    bool WorldToScreen(const Vector3& world, Vector2& screen, const ViewMatrix& matrix, int screenWidth, int screenHeight) {
        float w = matrix.m[3][0] * world.x + matrix.m[3][1] * world.y + matrix.m[3][2] * world.z + matrix.m[3][3];
        
        if (w < 0.001f)
            return false;

        float x = matrix.m[0][0] * world.x + matrix.m[0][1] * world.y + matrix.m[0][2] * world.z + matrix.m[0][3];
        float y = matrix.m[1][0] * world.x + matrix.m[1][1] * world.y + matrix.m[1][2] * world.z + matrix.m[1][3];

        float invW = 1.0f / w;
        x *= invW;
        y *= invW;

        screen.x = (screenWidth / 2.0f) + (x * screenWidth) / 2.0f;
        screen.y = (screenHeight / 2.0f) - (y * screenHeight) / 2.0f;

        return true;
    }

    void DrawBox(ImDrawList* drawList, float x, float y, float w, float h) {
        ImU32 color = boxColor;
        
        // Draw corner-style box
        float cornerLength = min(w, h) * 0.25f;
        
        // Top-left corner
        drawList->AddLine(ImVec2(x, y), ImVec2(x + cornerLength, y), color, boxThickness);
        drawList->AddLine(ImVec2(x, y), ImVec2(x, y + cornerLength), color, boxThickness);
        
        // Top-right corner
        drawList->AddLine(ImVec2(x + w, y), ImVec2(x + w - cornerLength, y), color, boxThickness);
        drawList->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + cornerLength), color, boxThickness);
        
        // Bottom-left corner
        drawList->AddLine(ImVec2(x, y + h), ImVec2(x + cornerLength, y + h), color, boxThickness);
        drawList->AddLine(ImVec2(x, y + h), ImVec2(x, y + h - cornerLength), color, boxThickness);
        
        // Bottom-right corner
        drawList->AddLine(ImVec2(x + w, y + h), ImVec2(x + w - cornerLength, y + h), color, boxThickness);
        drawList->AddLine(ImVec2(x + w, y + h), ImVec2(x + w, y + h - cornerLength), color, boxThickness);
    }

    void Render(ImDrawList* drawList, int screenWidth, int screenHeight) {
        if (!enabled) return;

        // Get client.dll base using SDK
        uintptr_t clientDll = (uintptr_t)GetModuleHandleA("client.dll");
        if (!clientDll)
            return;

        // Use SDK offsets - no hardcoding!
        uintptr_t entityList = Read<uintptr_t>(clientDll + cs2_dumper::offsets::client_dll::dwEntityList);
        ViewMatrix viewMatrix = Read<ViewMatrix>(clientDll + cs2_dumper::offsets::client_dll::dwViewMatrix);
        uintptr_t localPlayerPawn = Read<uintptr_t>(clientDll + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn);

        if (!entityList || !localPlayerPawn)
            return;

        // Get local team
        int localTeam = Read<int>(localPlayerPawn + 0x3E7); // m_iTeamNum

        int validPlayers = 0;

        // Iterate through entities
        for (int i = 0; i < 64; i++) {
            uintptr_t listEntry = Read<uintptr_t>(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
            if (!listEntry)
                continue;

            uintptr_t entity = Read<uintptr_t>(listEntry + 120 * (i & 0x1FF));
            if (!entity || entity == localPlayerPawn)
                continue;

            // Check health
            int health = Read<int>(entity + 0x34C); // m_iHealth
            if (health <= 0 || health > 100)
                continue;

            // Check team
            int team = Read<int>(entity + 0x3E7); // m_iTeamNum
            if (team == localTeam)
                continue; // Skip teammates

            validPlayers++;

            // Get game scene node for position
            uintptr_t gameSceneNode = Read<uintptr_t>(entity + 0x330); // m_pGameSceneNode
            if (!gameSceneNode)
                continue;

            Vector3 origin = Read<Vector3>(gameSceneNode + 0xC8); // m_vecAbsOrigin
            
            // Calculate head position (approximate)
            Vector3 headPos = origin;
            headPos.z += 72.0f; // Average player height

            // World to screen
            Vector2 screenFeet, screenHead;
            if (!WorldToScreen(origin, screenFeet, viewMatrix, screenWidth, screenHeight))
                continue;
            if (!WorldToScreen(headPos, screenHead, viewMatrix, screenWidth, screenHeight))
                continue;

            // Calculate box dimensions
            float height = screenFeet.y - screenHead.y;
            float width = height * 0.4f;
            
            float x = screenHead.x - width / 2.0f;
            float y = screenHead.y;

            // Draw the box
            DrawBox(drawList, x, y, width, height);
        }

        // Log player count periodically
        if (validPlayers != lastPlayerCount) {
            lastPlayerCount = validPlayers;
        }
    }

    void RenderMenu() {
        ImGui::Checkbox("Enable ESP", &enabled);

        if (enabled) {
            float color[4] = { 
                boxColor.Value.x, 
                boxColor.Value.y, 
                boxColor.Value.z, 
                boxColor.Value.w 
            };
            if (ImGui::ColorEdit4("Box Color", color)) {
                boxColor = ImColor(color[0], color[1], color[2], color[3]);
            }
            ImGui::SliderFloat("Box Thickness", &boxThickness, 1.0f, 5.0f);
        }
    }
}
