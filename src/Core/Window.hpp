#pragma once

#include <GLFW/glfw3.h>

#include <optional>
#include <tuple>

#include "Common/Common.hpp"
#include "Core/Layer/Layer.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

struct WindowProp {
  String name;
  size_t width = 800;
  size_t height = 600;
};

struct WindowData;
class Window : public std::enable_shared_from_this<Window> {
 public:
  explicit Window(const WindowProp& winProp);
  ~Window();

 public:
  void
  InitializeWindow();

  void
  ShowWindow();

  void
  SetUpEventCallBackFun();

  [[nodiscard]] GLFWwindow*
  GetGlfwWinow() const noexcept {
    return glfwWindow;
  }

  [[nodiscard]] std::tuple<uint32_t, uint32_t>
  GetWindowsSize() const;

  void
  SetCustomLayer(const std::shared_ptr<LayerBase>& customLayer);

  void
  InitLayer() {
    m_firstLayer->Attach();
  }

  [[nodiscard]] std::shared_ptr<RenderLayer>
  GetRenderLayer() const;

  [[nodiscard]] std::shared_ptr<ImguiLayer>
  GetImGuiLayer() const;

  std::shared_ptr<ResourceManager>
  GetResourceManager() const {
    return m_resourceManager;
  }

  RHIFactory*
  GetRHIFactory() const noexcept {
    return m_rhiFactory;
  }

 private:
  String m_windowName;
  GLFWwindow* glfwWindow;
  std::unique_ptr<WindowData> windowData;

  std::shared_ptr<LayerBase> m_firstLayer;
  RHIFactory* m_rhiFactory = nullptr;
  std::shared_ptr<ResourceManager> m_resourceManager = nullptr;
  std::shared_ptr<SwapChain> m_swapChain;

  // TODO: need add raii
  Vector<Semaphore> m_aviableSemaphore;
  Vector<Semaphore> m_renderFinishSemaphore;
  uint32_t m_frameIndex = 0;
  bool m_needResize = false;
};

}  // namespace Marbas
