#pragma once

#include <memory>

// #include "Core/RenderEngine.hpp"
#include "Common/Common.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

struct ApplicationData {
  RendererType rendererType = RendererType::VULKAN;
  String windowName = "Marbas.Editor";
  Path projectDir;
  int width = 800;
  int height = 600;
};

class Application final {
 public:
  Application(const ApplicationData& appData);

 public:
  void
  Initialize();

  void
  Run();

  void
  Quit();

 private:
  void
  BeginImgui();

  void
  BeginImGuiDocking();

  void
  EndImgui(int imageIndex, Semaphore* waitSemaphores, Semaphore* signalSemaphores);

  void
  EndImGuiDocking();

 private:
  std::unique_ptr<RHIFactory> m_rhiFactory;
  // std::unique_ptr<RenderEngine> m_renderEngine;
  std::unique_ptr<Scene> m_scene;

  GLFWwindow* m_glfwWindow;

  Swapchain* m_swapChain;
  std::vector<Semaphore*> m_aviableSemaphores;
  Semaphore* m_renderFinishSemaphore;
  Semaphore* m_imguiFinishSemaphore;
  std::vector<Semaphore*> m_finishSemaphores;
  Fence* m_frameFence;
  uint32_t m_currentFrame;

  ImguiContext* m_imguiContext;
};

}  // namespace Marbas
