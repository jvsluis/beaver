#pragma once

#include <GLFW/glfw3.h>

#include <string>

namespace bvr::app {

struct WindowDescriptor {
    std::string title = "Default Window";
    uint32_t width = 500;
    uint32_t height = 500;
    bool resizable = true;
};

struct WindowSize {
    uint32_t logical_width;
    uint32_t logical_height;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
};

class Window {
public:
    Window() = default;
    ~Window() = default;

    bool create(const WindowDescriptor& desc);
    void destroy();
    void update();
    bool should_close();

    GLFWwindow* handle() { return handle_; }
    WindowSize window_size() { return window_size_; }

private:
    GLFWwindow* handle_;
    WindowSize window_size_;
};

}  // namespace bvr::app
