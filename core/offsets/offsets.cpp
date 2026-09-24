#include <cstdint>

namespace offsets {
    // Client.dll offsets
    namespace client {
        uintptr_t dwLocalPlayerPawn = 0x0;
        uintptr_t dwEntityList = 0x0;
        uintptr_t dwViewMatrix = 0x0;
        uintptr_t dwViewAngles = 0x0;
        uintptr_t dwForceJump = 0x0;
        uintptr_t dwLocalPlayerController = 0x0;
    }

    // Engine2.dll offsets
    namespace engine {
        uintptr_t dwBuildNumber = 0x0;
        uintptr_t dwNetworkGameClient = 0x0;
    }

    // Player offsets
    namespace player {
        uintptr_t m_iHealth = 0x0;
        uintptr_t m_iTeamNum = 0x0;
        uintptr_t m_vecOrigin = 0x0;
        uintptr_t m_pGameSceneNode = 0x0;
        uintptr_t m_modelState = 0x0;
        uintptr_t m_vecViewOffset = 0x0;
        uintptr_t m_hPlayerPawn = 0x0;
        uintptr_t m_fFlags = 0x0;
        uintptr_t m_bIsScoped = 0x0;
    }

    // Weapon offsets
    namespace weapon {
        uintptr_t m_AttributeManager = 0x0;
        uintptr_t m_iItemDefinitionIndex = 0x0;
        uintptr_t m_nFallbackPaintKit = 0x0;
        uintptr_t m_nFallbackSeed = 0x0;
        uintptr_t m_flFallbackWear = 0x0;
    }
}
