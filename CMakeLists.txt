cmake_minimum_required(VERSION 3.28)
project(Nucleon)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenMP REQUIRED)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/SDL3/x86_64-w64-mingw32/lib/cmake")
find_package(SDL3 REQUIRED)

add_executable(Nucleon
        src/main.cpp
        src/particle_system.cpp
        src/spatial_grid.cpp
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/backends/imgui_impl_sdl3.cpp
        imgui/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(Nucleon PRIVATE
        src
        imgui
        imgui/backends
)
target_link_libraries(Nucleon PRIVATE SDL3::SDL3 OpenMP::OpenMP_CXX)

add_custom_command(TARGET Nucleon POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/SDL3/x86_64-w64-mingw32/bin/SDL3.dll"
        $<TARGET_FILE_DIR:Nucleon>
)