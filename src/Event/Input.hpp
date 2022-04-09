#ifndef MARBAS_EVENT_INPUT_H
#define MARBAS_EVENT_INPUT_H

#include "Common.hpp"
#include "Core/Application.hpp"

namespace Marbas {

class Input {
public:
    static bool IsKeyPress(int key) {
        auto* window = Application::GetApplicationsWindow()->GetGlfwWinow();
        int state = glfwGetKey(window, key);
        return state == GLFW_PRESS;
    }

    static bool IsMousePress(int button) {
        auto* window = Application::GetApplicationsWindow()->GetGlfwWinow();
        int state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }
};

}  // namespace Marbas

#endif
