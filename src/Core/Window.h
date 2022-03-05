#ifndef MARBARS_CORE_WINDOW_H
#define MARBARS_CORE_WINDOW_H

#include "Common.h"
#include "Layer/Layer.h"
#include "Layer/DrawLayer.h"
#include "Layer/RenderLayer.h"
#include "Event/Event.h"
#include "Layer/Widget/Image.h"

namespace Marbas {

struct WindowProp {
    folly::fbstring name;
    size_t width;
    size_t height;
};

struct WindowData;
class Window {
public:
    explicit Window(const WindowProp& winProp);
    ~Window();

public:
    void CreateSingleWindow();

    void ShowWindow();

    void SetUpEventCallBackFun();

    [[nodiscard]] GLFWwindow* GetGlfwWinow() const noexcept {
        return glfwWindow;
    }

    void RegisterLayer(Layer* layer);

    void RegisterLayers(const Vector<Layer*>& layers);

    void RegisterWidget(Widget* widget);

    void RegisterWidgets(const Vector<Widget*>& widgets);

    [[nodiscard]] Widget* GetWidget(const String& widgetName) const;

    [[nodiscard]] Layer* GetLayer(const String& layerName) const;

private:
    GLFWwindow* glfwWindow;
    WindowProp winProp;

    std::unique_ptr<WindowData> windowData;
    std::unique_ptr<Layer> firstLayer;
};

}  // namespace Marbas

#endif
