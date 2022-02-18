#ifndef MARBARS_CORE_WINDOW_H
#define MARBARS_CORE_WINDOW_H

#include "Core/Common.h"
#include "Layer/Layer.h"

#include <folly/FBString.h>
#include <glog/logging.h>

namespace Marbas::Core {

struct WindowProp {
    folly::fbstring name;
    size_t width;
    size_t height;
};

class Window {
    using Layer = Marbas::Layer::Layer;

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

    std::vector<Layer> layers;
};

}

#endif
