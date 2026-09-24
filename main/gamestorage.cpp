#include <Windows.h>
#include <cstdint>
#include <vector>

namespace gamestorage {
    // Game module handles
    HMODULE hClient = nullptr;
    HMODULE hEngine = nullptr;

    // Local player data
    struct LocalPlayer {
        uintptr_t address = 0;
        uintptr_t pawn = 0;
        int health = 0;
        int team = 0;
        bool isAlive = false;
        float position[3] = { 0.0f, 0.0f, 0.0f };
    } localPlayer;

    // Entity list storage
    struct Entity {
        uintptr_t address = 0;
        uintptr_t pawn = 0;
        int health = 0;
        int team = 0;
        bool isAlive = false;
        float position[3] = { 0.0f, 0.0f, 0.0f };
        float headPosition[3] = { 0.0f, 0.0f, 0.0f };
        bool isVisible = false;
    };

    std::vector<Entity> entityList;

    // View matrix for world to screen
    struct ViewMatrix {
        float matrix[4][4];
    } viewMatrix;

    // Screen dimensions
    struct Screen {
        int width = 0;
        int height = 0;
    } screen;

    // Utility functions
    void Initialize() {
        hClient = GetModuleHandleA("client.dll");
        hEngine = GetModuleHandleA("engine2.dll");

        RECT rect;
        GetWindowRect(GetForegroundWindow(), &rect);
        screen.width = rect.right - rect.left;
        screen.height = rect.bottom - rect.top;
    }

    void UpdateLocalPlayer() {
        // Update local player data each frame
        // This should be called from main loop
    }

    void UpdateEntityList() {
        // Update entity list each frame
        entityList.clear();
        // Populate entity list
    }

    void UpdateViewMatrix() {
        // Read view matrix from game memory
    }

    bool WorldToScreen(const float worldPos[3], float screenPos[2]) {
        // Transform 3D world position to 2D screen position
        // Using view matrix
        return false; // TODO: Implement
    }

    void ClearStorage() {
        entityList.clear();
        localPlayer = LocalPlayer{};
    }
}
