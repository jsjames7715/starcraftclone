cmake_minimum_required(VERSION 3.10)
project(StarCraftClone)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)

find_package(SDL2 QUIET)
if(NOT SDL2_FOUND)
    message(STATUS "SDL2 not found, OpenGL build disabled")
endif()

option(USE_OPENGL "Enable OpenGL rendering" OFF)

if(USE_OPENGL AND SDL2_FOUND)
    add_executable(starcraft src/main_gl.cpp src/gl_renderer.cpp)
    target_link_libraries(starcraft SDL2 GL GLU glut pthread)
else()
    add_executable(starcraft src/main.cpp)
    target_link_libraries(starcraft ncurses pthread)
endif()

install(TARGETS starcraft DESTINATION bin)
