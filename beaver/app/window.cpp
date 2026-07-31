#include "beaver/app/window.h"

#include "GLFW/glfw3.h"

namespace bvr::app {

bool Window::create(const WindowDescriptor& desc) {
    if (!glfwInit()) {
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (desc.resizable) {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }

    handle_ = glfwCreateWindow(desc.width, desc.height, desc.title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(handle_, this);

    // Query initial window sizing
    int fb_width = 0, fb_height = 0;
    glfwGetFramebufferSize(handle_, &fb_width, &fb_height);

    if (fb_width == 0 || fb_height == 0) {
        // Fallback to logical size if fb dims are zero (ex: the window starts minimized)
        fb_width = desc.width;
        fb_height = desc.height;
    }

    window_size_.logical_width = desc.width;
    window_size_.logical_height = desc.height;
    window_size_.framebuffer_width = fb_width;
    window_size_.framebuffer_height = fb_height;

    return true;
}

void Window::destroy() {
    if (handle_) {
        glfwDestroyWindow(handle_);
    }

    glfwTerminate();
}

void Window::update() {
    glfwPollEvents();
}

bool Window::should_close() {
    return (glfwWindowShouldClose(handle_) == GLFW_TRUE);
}

}  // namespace bvr::app
