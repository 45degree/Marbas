#ifndef MARBARS_CORE_WINDOW_H
#define MARBARS_CORE_WINDOW_H

#include "Common.h"
#include "Layer/Layer.h"
#include "Event/Event.h"

#include <folly/FBString.h>
#include <folly/FBVector.h>

namespace Marbas {

struct WindowProp {
    folly::fbstring name;
    size_t width;
    size_t height;
};

struct WindowData;
class Window {
public:
    Window(const WindowProp& winProp);
    ~Window();

public:
    void CreateWindow();

    void ShowWindow();

    void SetUpEventCallBackFun();

    GLFWwindow* GetGlfwWinow() const noexcept {
        return glfwWindow;
    }

private:
    GLFWwindow* glfwWindow;
    WindowProp winProp;

    std::unique_ptr<WindowData> windowData;
    std::unique_ptr<Layer> firstLayer;
};

}

#endif
