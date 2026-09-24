#pragma once

struct ImDrawList;

namespace esp {
    extern bool enabled;

    void Render(ImDrawList* drawList);
    void RenderMenu();
}
