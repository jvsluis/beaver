###############################################################################
# Beaver - A WebGPU game engine written in C++
# Author: James Vandersluis
# License: Apache License 2.0
###############################################################################
add_library(beaver
    app/application.cpp
    app/window.cpp

    core/log.cpp

    graphics/device.cpp
    graphics/renderer.cpp
    graphics/renderer2d.cpp
)

if(EMSCRIPTEN)
    target_link_libraries(beaver PUBLIC emdawnwebgpu_cpp webgpu_glfw)
    target_link_options(beaver PUBLIC "-sASYNCIFY=1" "-sUSE_GLFW=3")
else()
    target_include_directories(beaver PUBLIC beaver)
    target_link_libraries(beaver PUBLIC webgpu_dawn webgpu_glfw glfw)
endif()
