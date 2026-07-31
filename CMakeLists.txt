###############################################################################
# Beaver - A WebGPU game engine written in C++
# Author: James Vandersluis
# License: Apache License 2.0
###############################################################################
cmake_minimum_required(VERSION 3.20)
project(beaver)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(DAWN_USE_WAYLAND ON CACHE BOOL "Enable Wayland support" FORCE)
    set(DAWN_USE_X11 OFF CACHE BOOL "Disable X11 support" FORCE)
endif()

set(DAWN_FETCH_DEPENDENCIES ON)
add_subdirectory("dawn" EXCLUDE_FROM_ALL)

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	    1.0.3
)

FetchContent_MakeAvailable(glm)


# This fixes a dawn_glfw bug that uses the legacy EMSCRIPTEN definition
if (EMSCRIPTEN)
    target_compile_definitions(dawn_glfw PRIVATE EMSCRIPTEN=1)
endif()

include_directories(./)

add_subdirectory(beaver)
add_subdirectory(examples)
