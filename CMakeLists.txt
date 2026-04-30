cmake_minimum_required(VERSION 3.20)
project(ENICS2ESP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Static runtime (важно для андетекта!)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Все исходники
set(SOURCES
    src/main.cpp
    src/memory/memory.cpp
    src/game/entity.cpp
    src/overlay/overlay.cpp
    src/overlay/renderer.cpp
    src/utils/utils.cpp
)

# ImGui sources
set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/backends/imgui_impl_win32.cpp
    external/imgui/backends/imgui_impl_dx11.cpp
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${IMGUI_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    external/imgui
    external/imgui/backends
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    d3d11
    dxgi
    dwmapi
    d3dcompiler
)

# Release оптимизации
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2 /Ob2 /Oi /Ot /Oy /GL>
)
