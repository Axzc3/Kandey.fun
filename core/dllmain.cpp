#include <Windows.h>
#include <thread>

// Forward declaration from entry.cpp
void MainThread(HMODULE hModule);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        
        // Create main thread
        if (HANDLE hThread = CreateThread(nullptr, 0, 
            reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), 
            hModule, 0, nullptr)) {
            CloseHandle(hThread);
        }
        break;

    case DLL_PROCESS_DETACH:
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
