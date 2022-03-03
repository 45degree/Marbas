#include "Core/Window.h"
#include "Event/MouseEvent.h"
#include "Layer/ImguiLayer.h"
#include "Layer/Widget/Widget.h"
#include "Layer/Widget/MyWidget.h"
#include "Layer/Widget/Image.h"
#include "Layer/DockspaceLayer.h"
#include "Layer/DrawLayer.h"
#include "Layer/RenderLayer.h"
#include <unordered_map>

namespace Marbas {

struct WindowData {
    std::unique_ptr<EventCollection> eventCollection = nullptr;
    std::unordered_map<String, Layer*> LayerMap;
    std::unordered_map<String, Widget*> widgetMap;
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

void Window::RegisterWidget(Widget* widget) {
    const String& name = widget->GetWidgetName();
    windowData->widgetMap[name] = widget;
    LOG(INFO) << "register widget: " << name;
}

void Window::RegisterWidgets(const Vector<Widget*>& widgets) {
    for(auto* widget : widgets) {
        RegisterWidget(widget);
    }
}

void Window::CreateWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

    int width = static_cast<int>(winProp.width);
    int height = static_cast<int>(winProp.height);
    glfwWindow = glfwCreateWindow(width, height, winProp.name.c_str(), nullptr, nullptr);

    if(glfwWindow == nullptr) {
        throw std::runtime_error("failed to create window");
    }
    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1);

    glewExperimental = true;
    if(glewInit() != GLEW_OK) {
        LOG(ERROR) << "fail to initialize glew ";
    }

    // setup callback function
    SetUpEventCallBackFun();

    // attach layer
    auto imguiLayer = std::make_unique<ImguiLayer>(this);
    auto dockspaceLayer = std::make_unique<DockspaceLayer>();
    auto drawLayer = std::make_unique<DrawLayer>();
    auto renderLayer = std::make_unique<RenderLayer>();

    auto widget1 = std::make_unique<MyWidget>();
    auto widget2 = std::make_unique<Image>();
    RegisterWidgets({widget1.get(), widget2.get()});

    drawLayer->AddWidget(std::move(widget1));
    drawLayer->AddWidget(std::move(widget2));

    dockspaceLayer->AddNextLayer(std::move(drawLayer));
    imguiLayer->AddNextLayer(std::move(dockspaceLayer));
    renderLayer->AddNextLayer(std::move(imguiLayer));

    RegisterLayers({imguiLayer.get(), dockspaceLayer.get(), drawLayer.get(), renderLayer.get()});

    firstLayer = std::move(renderLayer);
    firstLayer->Attach();

    auto _renderLayer = dynamic_cast<RenderLayer*>(windowData->LayerMap.at("RenderLayer"));
    auto image = dynamic_cast<Image*>(windowData->widgetMap.at("Image"));
    auto textureId = const_cast<ImTextureID>(_renderLayer->GetFrameBufferTexture());
    image->ChangeTexture(textureId);

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

    glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height){
        // TODO(45degree): impl callback function
    });
}

}  // namespace Marbas
