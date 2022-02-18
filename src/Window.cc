#include "core/Window.h"
#include "glog/logging.h"

namespace Marbas::Core {

void Window::CreateWindow() {
    glfwWindow = glfwCreateWindow(winProp.width, winProp.height, winProp.name.c_str(),
                                  nullptr, nullptr);

    if(glfwWindow == nullptr) {
        std::runtime_error("failed to create window");
    }
}

}
