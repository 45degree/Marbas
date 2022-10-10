#pragma once

#include "RHI/Interface/ImguiInterface.hpp"

namespace Marbas {

class OpenGLImguiInterface final : public ImguiInterface {
 public:
  OpenGLImguiInterface() = default;
  ~OpenGLImguiInterface() override = default;

 public:
  void
  Resize(uint32_t width, uint32_t height) override {
    m_width = width;
    m_height = height;
  }

  void
  CreateImguiContext() override;

  void
  ClearUp() override;

  void
  SetUpImguiBackend(GLFWwindow* windows) override;

  void
  NewFrame() override;

  void
  RenderData(const Semaphore& semaphore, const Semaphore& signalSemaphore,
             uint32_t imageIndex) override;
};

}  // namespace Marbas
