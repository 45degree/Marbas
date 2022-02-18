#include "core/Application.h"
#include "core/Common.h"

#include <glog/logging.h>

namespace Marbas::Core {

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
    glfwMakeContextCurrent(glfwWindow);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(glfwWindow))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(glfwWindow);

        /* Poll for and process events */
        glfwPollEvents();
    }
}

}
