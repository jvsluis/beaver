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
