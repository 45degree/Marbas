#include "Core/Application.hpp"

#include "Common/Common.hpp"

namespace Marbas {

std::unique_ptr<Application> Application::app = nullptr;

void
Application::Init() {
  glfwSetErrorCallback([](int error, const char* descriptor) {
    LOG(ERROR) << FORMAT("glfw error {}: {}", error, descriptor);
  });

  if (!glfwInit()) {
    throw std::runtime_error("failed to initialized glfw");
  }

  m_isInitialized = true;
  LOG(INFO) << "initialized glfw successful!";
}

void
Application::CreateAppWindow(const WindowProp& winProp) {
  m_appWindow = std::make_unique<Window>(winProp);
  LOG(INFO) << "create window successful!";
}

void
Application::Run() {
  auto glfwWindow = m_appWindow->GetGlfwWinow();

  m_appWindow->InitLayer();
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(glfwWindow)) {
    /* Poll for and process events */
    glfwPollEvents();

    m_appWindow->ShowWindow();
  }
}

}  // namespace Marbas
