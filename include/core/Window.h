#ifndef MARBARS_CORE_WINDOW_H
#define MARBARS_CORE_WINDOW_H

#include "core/Common.h"
#include <folly/FBString.h>
#include <glog/logging.h>

namespace Marbas::Core {

struct WindowProp {
    folly::fbstring name;
    size_t width;
    size_t height;
};

class Window {
public:
    explicit Window(const WindowProp& winProp): winProp(winProp) {};

    ~Window() {
        glfwTerminate();
        LOG(INFO) << "destroy windows";
    }

public:
    void CreateWindow();

    GLFWwindow* GetGlfwWinow() const noexcept {
        return glfwWindow;
    }

private:
    GLFWwindow* glfwWindow;
    WindowProp winProp;
};

}

#endif
