#include "Core/Application.h"
#include "Common.h"

namespace Marbas {

std::unique_ptr<Application> Application::app;

void Application::Init() const {
    if(!glfwInit()) {
        throw std::runtime_error("failed to initialized glfw");
    }

    LOG(INFO) << "initialized glfw successful!";
}

void Application::CreateWindow(const WindowProp& winProp) {
    appWindow = std::make_unique<Window>(winProp);
    appWindow->CreateWindow();

    LOG(INFO) << "create window successful!";
}

void Application::Run() {
    auto glfwWindow = appWindow->GetGlfwWinow();

    auto vendor = glGetString(GL_VENDOR);
    auto version = glGetString(GL_VERSION);

    LOG(INFO) <<"vendor is:" << vendor;
    LOG(INFO) <<"version is:" << version;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(glfwWindow)) {
        /* Poll for and process events */
        glfwPollEvents();

        appWindow->ShowWindow();

        /* Swap front and back buffers */
        glfwSwapBuffers(glfwWindow);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

}  // namespace Marbas
