###############################################################################
# Beaver - A WebGPU game engine written in C++
# Author: James Vandersluis
# License: Apache License 2.0
###############################################################################
add_subdirectory(basic)

add_custom_target(copy_example_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMENT "Syncing example assets to build directory"
)
