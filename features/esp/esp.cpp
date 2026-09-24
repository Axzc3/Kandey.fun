#include "esp.h"
#include "../../main/gamestorage.h"
#include <d3d11.h>
#include "../../main/imgui/imgui.h"
#include "../../main/imgui/imgui_impl_dx11.h"
#include "../../main/imgui/imgui_impl_win32.h"

namespace esp {
    bool enabled = false;
    ImVec4 boxColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
    float boxThickness = 2.0f;

    void DrawBox(ImDrawList* drawList, float x, float y, float w, float h) {
        ImU32 color = ImGui::ColorConvertFloat4ToU32(boxColor);
        
        // Draw box outline
        drawList->AddRect(
            ImVec2(x, y),
            ImVec2(x + w, y + h),
            color,
            0.0f,
            0,
            boxThickness
        );
    }

    void Render(ImDrawList* drawList) {
        if (!enabled) return;

        // Iterate through entities from game storage
        for (const auto& entity : gamestorage::entityList) {
            if (!entity.isAlive) continue;
            if (entity.team == gamestorage::localPlayer.team) continue; // Skip teammates

            // Get screen position
            float screenPos[2];
            if (!gamestorage::WorldToScreen(entity.position, screenPos)) continue;

            // Calculate head position for box height
            float headScreen[2];
            if (!gamestorage::WorldToScreen(entity.headPosition, headScreen)) continue;

            // Calculate box dimensions
            float height = screenPos[1] - headScreen[1];
            float width = height * 0.4f; // Box width based on height
            
            float x = screenPos[0] - width / 2.0f;
            float y = headScreen[1];

            // Draw the box
            DrawBox(drawList, x, y, width, height);
        }
    }

    void RenderMenu() {
        if (ImGui::Checkbox("Enable ESP", &enabled)) {
            // ESP toggled
        }

        if (enabled) {
            ImGui::ColorEdit4("Box Color", (float*)&boxColor);
            ImGui::SliderFloat("Box Thickness", &boxThickness, 1.0f, 5.0f);
        }
    }
}
