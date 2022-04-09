#include "Core/Application.hpp"
#include "Common.hpp"

namespace Marbas {

std::unique_ptr<Application> Application::app;

void Application::Init() {
    glfwSetErrorCallback([](int error, const char* descriptor) {
        LOG(ERROR) << FORMAT("glfw error {}: {}", error, descriptor);
    });

    if(!glfwInit()) {
        throw std::runtime_error("failed to initialized glfw");
    }

    m_isInitialized = true;
    LOG(INFO) << "initialized glfw successful!";
}

void Application::CreateSingleWindow(const WindowProp& winProp) {
    appWindow = std::make_unique<Window>(winProp);
    appWindow->CreateSingleWindow();

    LOG(INFO) << "create window successful!";
}

void Application::Run() {
    auto glfwWindow = appWindow->GetGlfwWinow();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(glfwWindow)) {
        /* Poll for and process events */
        glfwPollEvents();

        appWindow->ShowWindow();
    }
}

}  // namespace Marbas
