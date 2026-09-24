#include <Windows.h>
#include <thread>
#include <iostream>
#include "../core/sdk/cpp/offsets.hpp"
#include "../features/esp/esp.h"

// Forward declarations
namespace core {
    void Initialize();
    void Shutdown();
}

namespace features {
    void InitializeFeatures();
    void ShutdownFeatures();
}

void MainThread(HMODULE hModule) {
    // Always allocate console for debugging
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "[+] Kandey.fun CS2 ESP Loaded!" << std::endl;
    std::cout << "[+] Press INSERT to toggle menu" << std::endl;
    std::cout << "[+] Press END to unload" << std::endl;

    // Initialize core systems
    core::Initialize();

    // Initialize features (ESP, Chams, etc.)
    features::InitializeFeatures();

    std::cout << "[+] All systems initialized" << std::endl;
    std::cout << "==========================================\n" << std::endl;

    // Main loop - wait for unload key and show stats
    auto lastUpdate = std::chrono::steady_clock::now();
    bool insertPressed = false;
    
    while (!(GetAsyncKeyState(VK_END) & 1)) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate).count();
        
        if (elapsed >= 1) {
            // Get player count from game
            uintptr_t clientDll = (uintptr_t)GetModuleHandleA("client.dll");
            int validPlayerCount = 0;
            
            if (clientDll) {
                // Read entity list and local player
                __try {
                    uintptr_t entityList = *(uintptr_t*)(clientDll + cs2_dumper::offsets::client_dll::dwEntityList);
                    uintptr_t localPlayerPawn = *(uintptr_t*)(clientDll + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn);
                    
                    if (entityList && localPlayerPawn) {
                        int localTeam = *(int*)(localPlayerPawn + 0x3E7);
                        
                        // Scan through entity indices
                        for (int i = 0; i < 64; i++) {
                            uintptr_t listEntry = *(uintptr_t*)(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
                            if (!listEntry) continue;
                            
                            uintptr_t entity = *(uintptr_t*)(listEntry + 120 * (i & 0x1FF));
                            if (!entity || entity == localPlayerPawn) continue;
                            
                            int health = *(int*)(entity + 0x34C);
                            if (health > 0 && health <= 100) {
                                int team = *(int*)(entity + 0x3E7);
                                if (team != localTeam && team > 0) {
                                    validPlayerCount++;
                                }
                            }
                        }
                    }
                }
                __except(EXCEPTION_EXECUTE_HANDLER) {
                    // Ignore memory read errors
                }
            }
            
            std::cout << "[INFO] Valid enemy players: " << validPlayerCount << " | Client.dll: " 
                      << (clientDll ? "Found" : "Not Found") << std::endl;
            
            lastUpdate = now;
        }
        
        // Handle INSERT key for ESP toggle (temporary - menu needs DX11 hooking)
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            if (!insertPressed) {
                esp::enabled = !esp::enabled;
                std::cout << "[TOGGLE] ESP: " << (esp::enabled ? "ON" : "OFF") << std::endl;
                insertPressed = true;
            }
        } else {
            insertPressed = false;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Cleanup
    std::cout << "\n[!] Unloading Kandey.fun..." << std::endl;
    features::ShutdownFeatures();
    core::Shutdown();

    if (f) fclose(f);
    FreeConsole();

    // Unload DLL
    FreeLibraryAndExitThread(hModule, 0);
}

namespace core {
    void Initialize() {
        std::cout << "[+] Core initialized" << std::endl;
        // Initialize patterns, offsets, memory reading, etc.
    }

    void Shutdown() {
        std::cout << "[-] Core shutdown" << std::endl;
        // Cleanup core systems
    }
}
