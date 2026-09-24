#include <Windows.h>
#include <vector>
#include <string>

namespace patterns {
    // Pattern scanning utility
    std::uintptr_t PatternScan(const char* moduleName, const char* signature) {
        HMODULE module = GetModuleHandleA(moduleName);
        if (!module)
            return 0;

        MODULEINFO moduleInfo;
        if (!GetModuleInformation(GetCurrentProcess(), module, &moduleInfo, sizeof(MODULEINFO)))
            return 0;

        auto base = reinterpret_cast<std::uintptr_t>(module);
        auto size = moduleInfo.SizeOfImage;

        auto pattern = std::vector<int>{};
        auto mask = std::string{};

        // Parse signature
        for (size_t i = 0; i < strlen(signature); i++) {
            if (signature[i] == ' ')
                continue;

            if (signature[i] == '?') {
                pattern.push_back(-1);
                mask += '?';
                if (signature[i + 1] == '?')
                    i++;
            }
            else {
                char byte[3] = { signature[i], signature[i + 1], 0 };
                pattern.push_back(strtol(byte, nullptr, 16));
                mask += 'x';
                i++;
            }
        }

        // Scan for pattern
        for (size_t i = 0; i < size - pattern.size(); i++) {
            bool found = true;
            for (size_t j = 0; j < pattern.size(); j++) {
                if (mask[j] == '?' || pattern[j] == *reinterpret_cast<int*>(base + i + j))
                    continue;

                found = false;
                break;
            }

            if (found)
                return base + i;
        }

        return 0;
    }

    // Client.dll patterns
    namespace client {
        const char* dwLocalPlayerPawn = "48 8B 05 ?? ?? ?? ?? 48 85 C0 74 ?? 8B 88";
        const char* dwEntityList = "48 8B 0D ?? ?? ?? ?? 48 89 7C 24 ?? 8B FA";
        const char* dwViewMatrix = "48 8D 0D ?? ?? ?? ?? 48 C1 E0 06";
        const char* dwViewAngles = "48 8B 0D ?? ?? ?? ?? E9 ?? ?? ?? ?? CC CC CC CC 40 55";
    }

    // Engine2.dll patterns
    namespace engine {
        const char* dwBuildNumber = "89 05 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? FF 15";
        const char* dwNetworkGameClient = "48 89 3D ?? ?? ?? ?? 48 8D 15";
    }

    void Initialize() {
        // Initialize all patterns on startup
        // This can be called to scan and populate offsets
    }
}
