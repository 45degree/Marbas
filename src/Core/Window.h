#ifndef MARBARS_CORE_WINDOW_H
#define MARBARS_CORE_WINDOW_H

#include "Common.h"
#include "Layer/Layer.h"
#include "Layer/DrawLayer.h"
#include "Layer/RenderLayer.h"
#include "Event/Event.h"
#include "Widget/RenderImage.h"

#include <tuple>

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

    // TODO: need to remove it
    void RegisterLayer(Layer* layer);
    void RegisterLayers(const Vector<Layer*>& layers);

    [[nodiscard]] Layer* GetLayer(const String& layerName) const;

    [[nodiscard]] std::tuple<uint32_t, uint32_t> GetWindowsSize() const;

private:
    String m_windowName;
    GLFWwindow* glfwWindow;
    std::unique_ptr<WindowData> windowData;

    std::unique_ptr<Layer> firstLayer;

    RHIFactory* m_rhiFactory;
};

}  // namespace Marbas

#endif
