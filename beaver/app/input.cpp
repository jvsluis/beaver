#include "beaver/app/input.h"

#include <GLFW/glfw3.h>

namespace bvr::app {

void* Input::s_window = nullptr;

void Input::init(void* window) {
    s_window = window;
}

bool Input::key_pressed(KeyCode keycode) {
    auto* window = static_cast<GLFWwindow*>(s_window);
    auto state = glfwGetKey(window, static_cast<int>(keycode));

    return (state == GLFW_PRESS || state == GLFW_REPEAT);
}

bool Input::mouse_button_pressed(MouseCode button) {
    auto* window = static_cast<GLFWwindow*>(s_window);
    auto state = glfwGetMouseButton(window, static_cast<int>(button));

    return state == GLFW_PRESS;
}

std::pair<float, float> Input::mouse_position() {
    auto* window = static_cast<GLFWwindow*>(s_window);
    double xpos, ypos;

    glfwGetCursorPos(window, &xpos, &ypos);

    return {(float)xpos, (float)ypos};
}

}  // namespace bvr::app
