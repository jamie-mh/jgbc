cmake_minimum_required(VERSION 3.7)
project(jgbc)

set(PROJECT_ROOT "${PROJECT_SOURCE_DIR}")
set(PROJECT_SOURCE_DIR "${PROJECT_ROOT}/src")
set(PROJECT_INCLUDE_DIR "${PROJECT_ROOT}/inc")
set(PROJECT_LIB_DIR "${PROJECT_ROOT}/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_ROOT}/bin")
set(OpenGL_GL_PREFERENCE "GLVND")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_definitions(-Wall -Wextra -O2)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_ROOT}/cmake/")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(
    jgbc
    ${PROJECT_SOURCE_DIR}/jgbc.c
    ${PROJECT_SOURCE_DIR}/alu.c
    ${PROJECT_SOURCE_DIR}/cpu.c
    ${PROJECT_SOURCE_DIR}/input.c
    ${PROJECT_SOURCE_DIR}/instr.c
    ${PROJECT_SOURCE_DIR}/mbc.c
    ${PROJECT_SOURCE_DIR}/ppu.c
    ${PROJECT_SOURCE_DIR}/mmu.c
    ${PROJECT_SOURCE_DIR}/cart.c
    ${PROJECT_SOURCE_DIR}/apu.c
    ${PROJECT_SOURCE_DIR}/main.c

    ${PROJECT_INCLUDE_DIR}/jgbc.h	
    ${PROJECT_INCLUDE_DIR}/alu.h
    ${PROJECT_INCLUDE_DIR}/cpu.h
    ${PROJECT_INCLUDE_DIR}/input.h
    ${PROJECT_INCLUDE_DIR}/instr.h
    ${PROJECT_INCLUDE_DIR}/mbc.h
    ${PROJECT_INCLUDE_DIR}/ppu.h
    ${PROJECT_INCLUDE_DIR}/mmu.h
    ${PROJECT_INCLUDE_DIR}/cart.h
    ${PROJECT_INCLUDE_DIR}/apu.h
)

include_directories(${SDL2_INCLUDE_DIR})
target_include_directories(jgbc PRIVATE ${PROJECT_INCLUDE_DIR})

add_executable(
    jgbc_debugger
    ${PROJECT_SOURCE_DIR}/jgbc.c
    ${PROJECT_SOURCE_DIR}/alu.c
    ${PROJECT_SOURCE_DIR}/cpu.c
    ${PROJECT_SOURCE_DIR}/input.c
    ${PROJECT_SOURCE_DIR}/instr.c
    ${PROJECT_SOURCE_DIR}/mbc.c
    ${PROJECT_SOURCE_DIR}/ppu.c
    ${PROJECT_SOURCE_DIR}/mmu.c
    ${PROJECT_SOURCE_DIR}/cart.c
    ${PROJECT_SOURCE_DIR}/apu.c

    ${PROJECT_INCLUDE_DIR}/jgbc.h	
    ${PROJECT_INCLUDE_DIR}/alu.h
    ${PROJECT_INCLUDE_DIR}/cpu.h
    ${PROJECT_INCLUDE_DIR}/input.h
    ${PROJECT_INCLUDE_DIR}/instr.h
    ${PROJECT_INCLUDE_DIR}/mbc.h
    ${PROJECT_INCLUDE_DIR}/ppu.h
    ${PROJECT_INCLUDE_DIR}/mmu.h
    ${PROJECT_INCLUDE_DIR}/cart.h
    ${PROJECT_INCLUDE_DIR}/apu.h
    
    ${PROJECT_SOURCE_DIR}/debug.cpp
	
    ${PROJECT_INCLUDE_DIR}/debug.h
    ${PROJECT_INCLUDE_DIR}/font.h

    ${PROJECT_LIB_DIR}/imgui/imgui.cpp
    ${PROJECT_LIB_DIR}/imgui/imgui_draw.cpp
    ${PROJECT_LIB_DIR}/imgui/imgui_demo.cpp
    ${PROJECT_LIB_DIR}/imgui/imgui_widgets.cpp

    ${PROJECT_LIB_DIR}/glad/glad.c
    ${PROJECT_LIB_DIR}/imgui/examples/imgui_impl_opengl3.cpp
    ${PROJECT_LIB_DIR}/imgui/examples/imgui_impl_sdl.cpp
)

set_property(TARGET jgbc_debugger PROPERTY CXX_STANDARD 11)
set_property(TARGET jgbc_debugger PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(jgbc_debugger PRIVATE ${PROJECT_INCLUDE_DIR})
target_include_directories(jgbc_debugger PUBLIC ${PROJECT_LIB_DIR})
target_include_directories(jgbc_debugger PUBLIC ${PROJECT_LIB_DIR}/imgui)

target_link_libraries(jgbc ${SDL2_LIBRARY})
target_link_libraries(jgbc_debugger ${SDL2_LIBRARY})
target_link_libraries(jgbc_debugger ${OPENGL_gl_LIBRARY})
target_link_libraries(jgbc_debugger ${CMAKE_DL_LIBS})
