###############################################################################
# Beaver - A WebGPU game engine written in C++
# Author: James Vandersluis
# License: Apache License 2.0
###############################################################################
add_executable(basic main.cpp)

add_dependencies(basic copy_example_assets)

if (EMSCRIPTEN)
    set_target_properties(basic PROPERTIES
        SUFFIX ".html"
    )
endif()

target_link_libraries(basic beaver)
