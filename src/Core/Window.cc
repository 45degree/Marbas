#include "Core/Window.h"
#include "Core/Application.h"
#include "Event/MouseEvent.h"
#include "Event/KeyEvent.h"
#include "Layer/ImguiLayer.h"
#include "Layer/DockspaceLayer.h"
#include "Layer/DrawLayer.h"
#include "Layer/RenderLayer.h"
#include "RHI/RHI.h"

#include <glog/logging.h>
#include <unordered_map>

namespace Marbas {

struct WindowData {
    uint32_t height;
    uint32_t width;
    std::unique_ptr<EventCollection> eventCollection = nullptr;
    std::unordered_map<String, Layer*> LayerMap;
};

Window::Window(const WindowProp& winProp):
        m_windowName(winProp.name),
        windowData(std::make_unique<WindowData>())
{
    windowData->width = winProp.width;
    windowData->height = winProp.height;
    windowData->eventCollection = std::make_unique<EventCollection>();
    m_rhiFactory = Application::GetRendererFactory();
}

Window::~Window() {
    firstLayer->Detach();

    glfwTerminate();
    // LOG(INFO) << "destroy windows";
}

void Window::RegisterLayer(Layer* layer) {
    if(layer == nullptr) return;

    const String& layerName = layer->GetLayerName();
    windowData->LayerMap[layerName] = layer;

    LOG(INFO) << "register layer: " << layerName;
}

void Window::RegisterLayers(const Vector<Layer*>& layers) {
    for(auto* layer : layers) {
        RegisterLayer(layer);
    }
}

void Window::CreateSingleWindow() {

    m_rhiFactory = Application::GetRendererFactory();

    int width = static_cast<int>(windowData->width);
    int height = static_cast<int>(windowData->height);
    glfwWindow = glfwCreateWindow(width, height, m_windowName.c_str(), nullptr, nullptr);

    if(glfwWindow == nullptr) {
        throw std::runtime_error("failed to create window");
    }

    m_rhiFactory->Init(glfwWindow);
    m_rhiFactory->PrintRHIInfo();

    // setup callback function
    SetUpEventCallBackFun();

    // attach layer
    auto imguiLayer = std::make_unique<ImguiLayer>(this);
    auto dockspaceLayer = std::make_unique<DockspaceLayer>();
    auto drawLayer = std::make_unique<DrawLayer>();
    auto renderLayer = std::make_unique<RenderLayer>(1920, 1080);
    RegisterLayers({imguiLayer.get(), dockspaceLayer.get(), drawLayer.get(), renderLayer.get()});

    dockspaceLayer->AddNextLayer(std::move(drawLayer));
    imguiLayer->AddNextLayer(std::move(dockspaceLayer));
    renderLayer->AddNextLayer(std::move(imguiLayer));

    firstLayer = std::move(renderLayer);
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

    // swap buffer
    m_rhiFactory->SwapBuffer(glfwWindow);

    // clear buffer
    m_rhiFactory->ClearBuffer(ClearBuferBit::COLOR_BUFFER);
}

Layer* Window::GetLayer(const String &layerName) const {
    auto& layers = windowData->LayerMap;
    if(layers.find(layerName) == layers.end()) return nullptr;

    return layers.at(layerName);
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
        GLFWkeyfun a;
    });

    glfwSetKeyCallback(glfwWindow,
                       [](GLFWwindow* glfwWindow, int key, int scancode, int action, int mods){
        auto windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(glfwWindow));
        if(action == GLFW_PRESS) {
            auto event = std::make_unique<KeyEvent>(EventType::MARBAS_KEY_PRESS_EVENT);
            windowData->eventCollection->AddEvent(std::move(event));
        }
        else if (action == GLFW_RELEASE) {
            auto event = std::make_unique<KeyEvent>(EventType::MARBAS_KEY_RELEASE_EVENT);
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

    glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* glfwWindow, int width, int height){
        auto windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(glfwWindow));
        windowData->width = width;
        windowData->height = height;
    });
}

std::tuple<uint32_t, uint32_t> Window::GetWindowsSize() const {
    return std::make_tuple(windowData->width, windowData->height);
}

}  // namespace Marbas
