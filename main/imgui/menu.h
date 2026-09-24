#pragma once
#include <d3d11.h>

namespace menu {
    extern bool showMenu;

    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
    void ToggleMenu();
    void Render();
    void Shutdown();
}
