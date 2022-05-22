#include "RHI/OpenGL/OpenGLSwapChain.hpp"

namespace Marbas {

OpenGLSwapChain::OpenGLSwapChain(GLFWwindow* glfwWindow, uint32_t width, uint32_t height)
    : SwapChain(width, height), m_glfwWindow(glfwWindow) {
  // create a default frame buffer
  FrameBufferInfo info{
      .width = width,
      .height = height,
      .renderPass = nullptr,
  };

  m_defaultFrameBuffer = std::make_shared<OpenGLFrameBuffer>(info);
}

OpenGLSwapChain::~OpenGLSwapChain() = default;

std::shared_ptr<FrameBuffer>
OpenGLSwapChain::GetDefaultFrameBuffer() {
  return m_defaultFrameBuffer;
}

void
OpenGLSwapChain::Present() {
  glfwSwapBuffers(m_glfwWindow);
}

}  // namespace Marbas
