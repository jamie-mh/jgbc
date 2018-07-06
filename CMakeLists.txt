cmake_minimum_required(VERSION 3.7)
project(lxgbc)

set(PROJECT_ROOT "${PROJECT_SOURCE_DIR}")
set(PROJECT_SOURCE_DIR "${PROJECT_ROOT}/src")
set(PROJECT_INCLUDE_DIR "${PROJECT_ROOT}/inc")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_ROOT}/bin")
set(OpenGL_GL_PREFERENCE "GLVND")

add_definitions(-Wall -Wextra -g)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(
    lxgbc
    ${PROJECT_SOURCE_DIR}/alu.c
    ${PROJECT_SOURCE_DIR}/cpu.c
    ${PROJECT_SOURCE_DIR}/emu.c
    ${PROJECT_SOURCE_DIR}/input.c
    ${PROJECT_SOURCE_DIR}/instr.c
    ${PROJECT_SOURCE_DIR}/lxgbc.c
    ${PROJECT_SOURCE_DIR}/mbc.c
    ${PROJECT_SOURCE_DIR}/ppu.c
    ${PROJECT_SOURCE_DIR}/ram.c
    ${PROJECT_SOURCE_DIR}/rom.c
    ${PROJECT_SOURCE_DIR}/sound.c
)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(lxgbc PRIVATE ${PROJECT_INCLUDE_DIR})
include_directories(lxgbc_debugger PRIVATE ${PROJECT_INCLUDE_DIR})

add_executable(
    lxgbc_debugger
    ${PROJECT_SOURCE_DIR}/alu.c
    ${PROJECT_SOURCE_DIR}/cpu.c
    ${PROJECT_SOURCE_DIR}/emu.c
    ${PROJECT_SOURCE_DIR}/input.c
    ${PROJECT_SOURCE_DIR}/instr.c
    ${PROJECT_SOURCE_DIR}/mbc.c
    ${PROJECT_SOURCE_DIR}/ppu.c
    ${PROJECT_SOURCE_DIR}/ram.c
    ${PROJECT_SOURCE_DIR}/rom.c
    ${PROJECT_SOURCE_DIR}/sound.c

    ${PROJECT_SOURCE_DIR}/debugger/debugger.cpp
    ${PROJECT_SOURCE_DIR}/debugger/glad.c
    ${PROJECT_SOURCE_DIR}/debugger/imgui_impl_opengl3.cpp
    ${PROJECT_SOURCE_DIR}/debugger/imgui_impl_sdl.cpp
    ${PROJECT_SOURCE_DIR}/debugger/window_emu.cpp

    ${PROJECT_ROOT}/imgui/imgui.cpp
    ${PROJECT_ROOT}/imgui/imgui_draw.cpp
    ${PROJECT_ROOT}/imgui/imgui_demo.cpp
)
set_property(TARGET lxgbc_debugger PROPERTY CXX_STANDARD 11)
set_property(TARGET lxgbc_debugger PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(lxgbc_debugger PUBLIC "${PROJECT_ROOT}/imgui")
target_include_directories(lxgbc_debugger PUBLIC "${PROJECT_INCLUDE_DIR}/debugger")

target_link_libraries(lxgbc ${SDL2_LIBRARIES})
target_link_libraries(lxgbc_debugger ${SDL2_LIBRARIES})
target_link_libraries(lxgbc_debugger ${OPENGL_gl_LIBRARY})
target_link_libraries(lxgbc_debugger ${CMAKE_DL_LIBS})