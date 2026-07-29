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

class Window {
public:
    Window() = default;
    ~Window() = default;

    bool create(const WindowDescriptor& desc);
    void destroy();
    void update();
    bool should_close();

private:
    GLFWwindow* handle_;
};

}  // namespace bvr::app
