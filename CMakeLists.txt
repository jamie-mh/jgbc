cmake_minimum_required(VERSION 3.7)
project(jgbc)

set(PROJECT_ROOT "${PROJECT_SOURCE_DIR}")
set(PROJECT_SOURCE_DIR "${PROJECT_ROOT}/src")
set(PROJECT_INCLUDE_DIR "${PROJECT_ROOT}/inc")
set(PROJECT_LIB_DIR "${PROJECT_ROOT}/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_ROOT}/bin")
set(OpenGL_GL_PREFERENCE "GLVND")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_ROOT}/cmake/")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(
    jgbc
    ${PROJECT_SOURCE_DIR}/jgbc.c
    ${PROJECT_SOURCE_DIR}/gameboy.c
    ${PROJECT_SOURCE_DIR}/alu.c
    ${PROJECT_SOURCE_DIR}/cpu.c
    ${PROJECT_SOURCE_DIR}/input.c
    ${PROJECT_SOURCE_DIR}/instr.c
    ${PROJECT_SOURCE_DIR}/mbc.c
    ${PROJECT_SOURCE_DIR}/ppu.c
    ${PROJECT_SOURCE_DIR}/mmu.c
    ${PROJECT_SOURCE_DIR}/cart.c
    ${PROJECT_SOURCE_DIR}/apu.c

    ${PROJECT_INCLUDE_DIR}/gameboy.h
    ${PROJECT_INCLUDE_DIR}/alu.h
    ${PROJECT_INCLUDE_DIR}/cpu.h
    ${PROJECT_INCLUDE_DIR}/input.h
    ${PROJECT_INCLUDE_DIR}/instr.h
    ${PROJECT_INCLUDE_DIR}/mbc.h
    ${PROJECT_INCLUDE_DIR}/ppu.h
    ${PROJECT_INCLUDE_DIR}/mmu.h
    ${PROJECT_INCLUDE_DIR}/cart.h
    ${PROJECT_INCLUDE_DIR}/apu.h
    ${PROJECT_INCLUDE_DIR}/macro.h
)

include_directories(${SDL2_INCLUDE_DIR})
target_include_directories(jgbc PRIVATE ${PROJECT_INCLUDE_DIR})
target_include_directories(jgbc PRIVATE ${PROJECT_LIB_DIR}/stb)

add_executable(
    jgbc_debugger
    ${PROJECT_SOURCE_DIR}/gameboy.c
    ${PROJECT_SOURCE_DIR}/alu.c
    ${PROJECT_SOURCE_DIR}/cpu.c
    ${PROJECT_SOURCE_DIR}/input.c
    ${PROJECT_SOURCE_DIR}/instr.c
    ${PROJECT_SOURCE_DIR}/mbc.c
    ${PROJECT_SOURCE_DIR}/ppu.c
    ${PROJECT_SOURCE_DIR}/mmu.c
    ${PROJECT_SOURCE_DIR}/cart.c
    ${PROJECT_SOURCE_DIR}/apu.c

    ${PROJECT_INCLUDE_DIR}/gameboy.h
    ${PROJECT_INCLUDE_DIR}/alu.h
    ${PROJECT_INCLUDE_DIR}/cpu.h
    ${PROJECT_INCLUDE_DIR}/input.h
    ${PROJECT_INCLUDE_DIR}/instr.h
    ${PROJECT_INCLUDE_DIR}/mbc.h
    ${PROJECT_INCLUDE_DIR}/ppu.h
    ${PROJECT_INCLUDE_DIR}/mmu.h
    ${PROJECT_INCLUDE_DIR}/cart.h
    ${PROJECT_INCLUDE_DIR}/apu.h
    ${PROJECT_INCLUDE_DIR}/macro.h    
    
    ${PROJECT_SOURCE_DIR}/debugger/jgbc.cpp
    ${PROJECT_SOURCE_DIR}/debugger/debugger.cpp
    ${PROJECT_SOURCE_DIR}/debugger/colours.cpp
    ${PROJECT_SOURCE_DIR}/debugger/menubar.cpp
    ${PROJECT_SOURCE_DIR}/debugger/window.cpp
    ${PROJECT_SOURCE_DIR}/debugger/windows/breakpoints.cpp
    ${PROJECT_SOURCE_DIR}/debugger/windows/cart_info.cpp
    ${PROJECT_SOURCE_DIR}/debugger/windows/controls.cpp
    ${PROJECT_SOURCE_DIR}/debugger/windows/disassembly.cpp
    ${PROJECT_SOURCE_DIR}/debugger/windows/framebuffer.cpp
    ${PROJECT_SOURCE_DIR}/debugger/windows/io.cpp
    ${PROJECT_SOURCE_DIR}/debugger/windows/memory.cpp
    ${PROJECT_SOURCE_DIR}/debugger/windows/palettes.cpp
    ${PROJECT_SOURCE_DIR}/debugger/windows/registers.cpp
    ${PROJECT_SOURCE_DIR}/debugger/windows/serial.cpp
    ${PROJECT_SOURCE_DIR}/debugger/windows/stack.cpp

    ${PROJECT_INCLUDE_DIR}/debugger/debugger.h
    ${PROJECT_INCLUDE_DIR}/debugger/emulator.h
    ${PROJECT_INCLUDE_DIR}/debugger/font.h
    ${PROJECT_INCLUDE_DIR}/debugger/colours.h
    ${PROJECT_INCLUDE_DIR}/debugger/menubar.h
    ${PROJECT_INCLUDE_DIR}/debugger/window.h
    ${PROJECT_INCLUDE_DIR}/debugger/windows/breakpoints.h
    ${PROJECT_INCLUDE_DIR}/debugger/windows/cart_info.h
    ${PROJECT_INCLUDE_DIR}/debugger/windows/controls.h
    ${PROJECT_INCLUDE_DIR}/debugger/windows/disassembly.h
    ${PROJECT_INCLUDE_DIR}/debugger/windows/framebuffer.h
    ${PROJECT_INCLUDE_DIR}/debugger/windows/io.h
    ${PROJECT_INCLUDE_DIR}/debugger/windows/memory.h
    ${PROJECT_INCLUDE_DIR}/debugger/windows/palettes.h
    ${PROJECT_INCLUDE_DIR}/debugger/windows/registers.h
    ${PROJECT_INCLUDE_DIR}/debugger/windows/serial.h
    ${PROJECT_INCLUDE_DIR}/debugger/windows/stack.h

    ${PROJECT_LIB_DIR}/imgui/imgui.cpp
    ${PROJECT_LIB_DIR}/imgui/imgui_draw.cpp
    ${PROJECT_LIB_DIR}/imgui/imgui_demo.cpp
    ${PROJECT_LIB_DIR}/imgui/imgui_widgets.cpp
    ${PROJECT_LIB_DIR}/imgui/imgui_tables.cpp

    ${PROJECT_LIB_DIR}/glad/glad.c
    ${PROJECT_LIB_DIR}/imgui/backends/imgui_impl_opengl3.cpp
    ${PROJECT_LIB_DIR}/imgui/backends/imgui_impl_sdl2.cpp
)

set_property(TARGET jgbc_debugger PROPERTY CXX_STANDARD 17)
set_property(TARGET jgbc_debugger PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(jgbc_debugger PRIVATE ${PROJECT_INCLUDE_DIR})
target_include_directories(jgbc_debugger PRIVATE ${PROJECT_LIB_DIR})
target_include_directories(jgbc_debugger PRIVATE ${PROJECT_LIB_DIR}/imgui)
target_include_directories(jgbc_debugger PRIVATE ${PROJECT_LIB_DIR}/imgui/backends)
target_include_directories(jgbc_debugger PRIVATE ${PROJECT_LIB_DIR}/imgui_club)

target_link_libraries(jgbc ${SDL2_LIBRARY})
target_link_libraries(jgbc_debugger ${SDL2_LIBRARY})
target_link_libraries(jgbc_debugger ${OPENGL_gl_LIBRARY})
target_link_libraries(jgbc_debugger ${CMAKE_DL_LIBS})
