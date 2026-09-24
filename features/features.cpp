#include <iostream>
#include "esp/esp.h"

namespace features {
    void InitializeFeatures() {
        std::cout << "[+] Features initialized" << std::endl;
        // Features are initialized but rendering happens in the render hook
    }

    void ShutdownFeatures() {
        std::cout << "[-] Features shutdown" << std::endl;
        // Cleanup features
    }
}
