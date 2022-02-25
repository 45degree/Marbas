#include "Core/Window.h"

#include "Event/MouseEvent.h"
#include "Layer/ImguiLayer.h"
#include "Layer/Widget/MyWidget.h"
#include "Layer/DockspaceLayer.h"
#include "Layer/DrawLayer.h"

namespace Marbas {

struct WindowData {
    std::unique_ptr<EventCollection> eventCollection = nullptr;
};

Window::Window(const WindowProp& winProp):
        winProp(winProp),
        windowData(std::make_unique<WindowData>())
{
    windowData->eventCollection = std::make_unique<EventCollection>();
}


Window::~Window() {
    firstLayer->Detach();

    glfwTerminate();
    LOG(INFO) << "destroy windows";
}

void Window::CreateWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

    glfwWindow = glfwCreateWindow(winProp.width, winProp.height, winProp.name.c_str(),
                                  nullptr, nullptr);

    if(glfwWindow == nullptr) {
        throw std::runtime_error("failed to create window");
    }
    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1);

    // setup callback function
    SetUpEventCallBackFun();

    // attach layer
    auto imguiLayer = std::make_unique<ImguiLayer>(this);
    auto dockspaceLayer = std::make_unique<DockspaceLayer>();
    auto drawLayer = std::make_unique<DrawLayer>();
    auto widget = std::make_unique<MyWidget>("title1");
    drawLayer->AddWidget(std::move(widget));

    dockspaceLayer->AddNextLayer(std::move(drawLayer));
    imguiLayer->AddNextLayer(std::move(dockspaceLayer));

    firstLayer = std::move(imguiLayer);

    firstLayer->Attach();
    glfwSetWindowUserPointer(glfwWindow, windowData.get());
}

void Window::ShowWindow() {

    // push all event to every layer
    windowData->eventCollection->BroadcastEventFromLayer(firstLayer.get());
    windowData->eventCollection->ClearEvent();

    firstLayer->Begin();
    firstLayer->Update();
    firstLayer->End();
}

void Window::SetUpEventCallBackFun() {
    glfwSetCursorPosCallback(glfwWindow,  [](GLFWwindow* glfwWindow, double xpos, double ypos) {
        auto windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(glfwWindow));
        auto event = std::make_unique<MouseMoveEvent>();
        event->SetPos(xpos, ypos);
        windowData->eventCollection->AddEvent(std::move(event));
    });

    glfwSetMouseButtonCallback(glfwWindow,
                               [](GLFWwindow* glfwWindow, int button, int action, int mods) {
        auto windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(glfwWindow));
        auto event = std::make_unique<MouseMoveEvent>();

        double xpos, ypos;
        glfwGetCursorPos(glfwWindow, &xpos, &ypos);
        if(action == GLFW_PRESS) {
            auto event = std::make_unique<MousePressEvent>(button);
            event->SetPos(xpos, ypos);
            windowData->eventCollection->AddEvent(std::move(event));
        }
        else if(action == GLFW_RELEASE) {
            auto event = std::make_unique<MouseReleaseEvent>(button);
            event->SetPos(xpos, ypos);
            windowData->eventCollection->AddEvent(std::move(event));
        }
    });

    glfwSetScrollCallback(glfwWindow, [](GLFWwindow* glfwWindow, double xOffset, double yOffset) {
        auto windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(glfwWindow));

        double xpos, ypos;
        glfwGetCursorPos(glfwWindow, &xpos, &ypos);

        auto event = std::make_unique<MouseScrolledEvent>(xOffset, yOffset);
        event->SetPos(xpos, ypos);

        windowData->eventCollection->AddEvent(std::move(event));
    });
}

}  // namespace Marbas
