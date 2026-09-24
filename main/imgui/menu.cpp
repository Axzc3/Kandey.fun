#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "../../features/esp/esp.h"

namespace menu {
    bool showMenu = true;
    bool initialized = false;

    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context) {
        if (initialized) return;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        // Setup style
        ImGui::StyleColorsDark();

        initialized = true;
    }

    void ToggleMenu() {
        showMenu = !showMenu;
    }

    void Render() {
        if (!showMenu) return;

        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        ImGui::Begin("Kandey.fun - CS2 Overlay", &showMenu, ImGuiWindowFlags_NoCollapse);

        if (ImGui::BeginTabBar("MainTabs")) {
            if (ImGui::BeginTabItem("ESP")) {
                ImGui::Spacing();
                esp::RenderMenu();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Misc")) {
                ImGui::Text("Coming soon...");
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    void Shutdown() {
        if (!initialized) return;
        
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        
        initialized = false;
    }
}
