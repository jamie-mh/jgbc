cmake_minimum_required(VERSION 3.7)
project(jgbc)

set(PROJECT_ROOT "${PROJECT_SOURCE_DIR}")
set(PROJECT_SOURCE_DIR "${PROJECT_ROOT}/src")
set(PROJECT_INCLUDE_DIR "${PROJECT_ROOT}/inc")
set(PROJECT_LIB_DIR "${PROJECT_ROOT}/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_ROOT}/bin")
set(OpenGL_GL_PREFERENCE "GLVND")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	add_definitions(-Wall -Wextra -g)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_ROOT}/cmake/")

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(
    jgbc
    ${PROJECT_SOURCE_DIR}/alu.c
    ${PROJECT_SOURCE_DIR}/cpu.c
    ${PROJECT_SOURCE_DIR}/emu.c
    ${PROJECT_SOURCE_DIR}/input.c
    ${PROJECT_SOURCE_DIR}/instr.c
    ${PROJECT_SOURCE_DIR}/jgbc.c
    ${PROJECT_SOURCE_DIR}/mbc.c
    ${PROJECT_SOURCE_DIR}/ppu.c
    ${PROJECT_SOURCE_DIR}/ram.c
    ${PROJECT_SOURCE_DIR}/cart.c
	
    ${PROJECT_INCLUDE_DIR}/apu.h
	${PROJECT_INCLUDE_DIR}/alu.h
    ${PROJECT_INCLUDE_DIR}/cpu.h
    ${PROJECT_INCLUDE_DIR}/emu.h
    ${PROJECT_INCLUDE_DIR}/input.h
    ${PROJECT_INCLUDE_DIR}/instr.h
    ${PROJECT_INCLUDE_DIR}/jgbc.h
    ${PROJECT_INCLUDE_DIR}/mbc.h
    ${PROJECT_INCLUDE_DIR}/ppu.h
    ${PROJECT_INCLUDE_DIR}/ram.h
    ${PROJECT_INCLUDE_DIR}/cart.h
    ${PROJECT_INCLUDE_DIR}/apu.h
)

include_directories(${SDL2_INCLUDE_DIR})
target_include_directories(jgbc PRIVATE ${PROJECT_INCLUDE_DIR})

add_executable(
    jgbc_debugger
    ${PROJECT_SOURCE_DIR}/alu.c
    ${PROJECT_SOURCE_DIR}/cpu.c
    ${PROJECT_SOURCE_DIR}/emu.c
    ${PROJECT_SOURCE_DIR}/input.c
    ${PROJECT_SOURCE_DIR}/instr.c
    ${PROJECT_SOURCE_DIR}/mbc.c
    ${PROJECT_SOURCE_DIR}/ppu.c
    ${PROJECT_SOURCE_DIR}/ram.c
    ${PROJECT_SOURCE_DIR}/cart.c
    ${PROJECT_SOURCE_DIR}/apu.c
	
	${PROJECT_INCLUDE_DIR}/apu.h
	${PROJECT_INCLUDE_DIR}/alu.h
    ${PROJECT_INCLUDE_DIR}/cpu.h
    ${PROJECT_INCLUDE_DIR}/emu.h
    ${PROJECT_INCLUDE_DIR}/input.h
    ${PROJECT_INCLUDE_DIR}/instr.h
    ${PROJECT_INCLUDE_DIR}/jgbc.h
    ${PROJECT_INCLUDE_DIR}/mbc.h
    ${PROJECT_INCLUDE_DIR}/ppu.h
    ${PROJECT_INCLUDE_DIR}/ram.h
    ${PROJECT_INCLUDE_DIR}/cart.h
    ${PROJECT_INCLUDE_DIR}/apu.h
    
    ${PROJECT_SOURCE_DIR}/debugger/debugger.cpp
    ${PROJECT_SOURCE_DIR}/debugger/menu.cpp
    ${PROJECT_SOURCE_DIR}/debugger/window_emu.cpp
    ${PROJECT_SOURCE_DIR}/debugger/window_perf.cpp
    ${PROJECT_SOURCE_DIR}/debugger/window_disasm.cpp
    ${PROJECT_SOURCE_DIR}/debugger/window_reg.cpp
    ${PROJECT_SOURCE_DIR}/debugger/window_ctrl.cpp
    ${PROJECT_SOURCE_DIR}/debugger/window_mem.cpp
    ${PROJECT_SOURCE_DIR}/debugger/window_break.cpp
    ${PROJECT_SOURCE_DIR}/debugger/window_stack.cpp
    ${PROJECT_SOURCE_DIR}/debugger/modal_rom.cpp
	
	${PROJECT_INCLUDE_DIR}/debugger/debugger.h
    ${PROJECT_INCLUDE_DIR}/debugger/menu.h
    ${PROJECT_INCLUDE_DIR}/debugger/window_emu.h
    ${PROJECT_INCLUDE_DIR}/debugger/window_perf.h
    ${PROJECT_INCLUDE_DIR}/debugger/window_disasm.h
    ${PROJECT_INCLUDE_DIR}/debugger/window_reg.h
    ${PROJECT_INCLUDE_DIR}/debugger/window_ctrl.h
    ${PROJECT_INCLUDE_DIR}/debugger/window_mem.h
    ${PROJECT_INCLUDE_DIR}/debugger/window_break.h
    ${PROJECT_INCLUDE_DIR}/debugger/window_stack.h
    ${PROJECT_INCLUDE_DIR}/debugger/modal_rom.h
	${PROJECT_INCLUDE_DIR}/debugger/liberation_mono.h

    ${PROJECT_LIB_DIR}/imgui/imgui.cpp
    ${PROJECT_LIB_DIR}/imgui/imgui_draw.cpp
    ${PROJECT_LIB_DIR}/imgui/imgui_demo.cpp

    ${PROJECT_LIB_DIR}/glad/glad.c
    ${PROJECT_LIB_DIR}/imgui_impl/imgui_impl_opengl3.cpp
    ${PROJECT_LIB_DIR}/imgui_impl/imgui_impl_sdl.cpp
    ${PROJECT_LIB_DIR}/imgui_dock/imgui_dock.cpp
    ${PROJECT_LIB_DIR}/imgui_plot/imgui_plot.cpp
)
set_property(TARGET jgbc_debugger PROPERTY CXX_STANDARD 11)
set_property(TARGET jgbc_debugger PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(jgbc_debugger PRIVATE ${PROJECT_INCLUDE_DIR})
target_include_directories(jgbc_debugger PUBLIC ${PROJECT_LIB_DIR})

target_link_libraries(jgbc ${SDL2_LIBRARY})
target_link_libraries(jgbc_debugger ${SDL2_LIBRARY})
target_link_libraries(jgbc_debugger ${OPENGL_gl_LIBRARY})
target_link_libraries(jgbc_debugger ${CMAKE_DL_LIBS})
