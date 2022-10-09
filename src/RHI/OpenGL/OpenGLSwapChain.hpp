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

  int
  Present(const Vector<Semaphore>& waitSemaphores, uint32_t imageIndex) override;

  void
  Resize(uint32_t width, uint32_t height) override;

  size_t
  GetImageCount() const override {
    return 1;
  }

  int
  AcquireNextImage(const Semaphore& semaphore) override {
    return 1;
  }

 private:
  std::shared_ptr<OpenGLFrameBuffer> m_defaultFrameBuffer = nullptr;
  GLFWwindow* m_glfwWindow;
};

}  // namespace Marbas
