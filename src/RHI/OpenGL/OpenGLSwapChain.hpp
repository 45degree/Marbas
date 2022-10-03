#pragma once

#include "OpenGLFrameBuffer.hpp"
#include "RHI/Interface/SwapChain.hpp"

namespace Marbas {

class OpenGLSwapChain final : public SwapChain {
 public:
  OpenGLSwapChain(GLFWwindow* glfwWindow, uint32_t width, uint32_t height);
  ~OpenGLSwapChain();

 public:
  std::shared_ptr<FrameBuffer>
  GetDefaultFrameBuffer() override;

  void
  Present() override;

  void
  Update(uint32_t width, uint32_t height) override;

 private:
  std::shared_ptr<OpenGLFrameBuffer> m_defaultFrameBuffer = nullptr;
  GLFWwindow* m_glfwWindow;
};

}  // namespace Marbas
