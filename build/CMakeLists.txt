cmake_minimum_required(VERSION 3.15)
project(KandeyESP)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ImGui source files
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/main/imgui/imgui-master/imgui-master")

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
)

# Project source files
set(PROJECT_SOURCES
    core/dllmain.cpp
    core/entry.cpp
    core/patterns/patterns.cpp
    features/esp/esp.cpp
    features/esp/chams.cpp
    main/gamestorage.cpp
    main/imgui/menu.cpp
    ${IMGUI_SOURCES}
)

# Create DLL
add_library(KandeyESP SHARED ${PROJECT_SOURCES})

# Include directories
target_include_directories(KandeyESP PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_SOURCE_DIR}/core/sdk/cpp
)

# Link libraries
target_link_libraries(KandeyESP PRIVATE
    d3d11
    user32
    gdi32
)

# Compiler flags for Release build optimization
if(MSVC)
    target_compile_options(KandeyESP PRIVATE
        $<$<CONFIG:Release>:/O2 /GL>
    )
    target_link_options(KandeyESP PRIVATE
        $<$<CONFIG:Release>:/LTCG>
    )
endif()

# Output to build directory
set_target_properties(KandeyESP PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build
)
