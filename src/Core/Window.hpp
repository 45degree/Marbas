#ifndef MARBARS_CORE_WINDOW_H
#define MARBARS_CORE_WINDOW_H

#include <GLFW/glfw3.h>

#include <tuple>

#include "Common.hpp"
#include "Layer/Layer.hpp"
#include "RHI/RHI.hpp"

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

  [[nodiscard]] GLFWwindow* GetGlfwWinow() const noexcept { return glfwWindow; }

  // [[nodiscard]] LayerBase* GetLayer(const String& layerName) const;

  [[nodiscard]] std::tuple<uint32_t, uint32_t> GetWindowsSize() const;

  [[nodiscard]] RenderLayer* GetRenderLayer() const;
  [[nodiscard]] DrawLayer* GetDrawLayer() const;
  [[nodiscard]] ImguiLayer* GetImGuiLayer() const;

 private:
  String m_windowName;
  GLFWwindow* glfwWindow;
  std::unique_ptr<WindowData> windowData;

  LayerBase* firstLayer;
  RHIFactory* m_rhiFactory;
};

}  // namespace Marbas

#endif
