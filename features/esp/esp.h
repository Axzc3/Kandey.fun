#pragma once

struct ImDrawList;
struct ImColor;

namespace esp {
    extern bool enabled;
    extern ImColor boxColor;
    extern float boxThickness;

    void Render(ImDrawList* drawList, int screenWidth, int screenHeight);
    void RenderMenu();
}
