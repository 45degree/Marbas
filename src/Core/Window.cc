#include "Core/Window.h"
#include "Core/Application.h"
#include "Event/MouseEvent.h"
#include "Event/KeyEvent.h"
#include "Layer/ImguiLayer.h"
#include "Layer/Widget/Widget.h"
// #include "Layer/Widget/MyWidget.h"
#include "Layer/Widget/FileDialog.h"
#include "Layer/Widget/Image.h"
#include "Layer/DockspaceLayer.h"
#include "Layer/DrawLayer.h"
#include "Layer/RenderLayer.h"
#include "RHI/RHI.h"

#include <unordered_map>

namespace Marbas {

struct WindowData {
    uint32_t height;
    uint32_t width;
    std::unique_ptr<EventCollection> eventCollection = nullptr;
    std::unordered_map<String, Layer*> LayerMap;
    std::unordered_map<String, Widget*> widgetMap;
};

Window::Window(const WindowProp& winProp):
        windowData(std::make_unique<WindowData>()),
        m_windowName(winProp.name)
{
    windowData->width = winProp.width;
    windowData->height = winProp.height;
    windowData->eventCollection = std::make_unique<EventCollection>();
    m_rhiFactory = Application::GetRendererFactory();
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

void Window::CreateSingleWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

    int width = static_cast<int>(windowData->width);
    int height = static_cast<int>(windowData->height);
    glfwWindow = glfwCreateWindow(width, height, m_windowName.c_str(), nullptr, nullptr);

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
    RegisterLayers({imguiLayer.get(), dockspaceLayer.get(), drawLayer.get(), renderLayer.get()});

    auto viewport = m_rhiFactory->CreateViewport();
    viewport->SetViewport(0, 0, width, height);

    FileDialogCrateInfo info {
        "TextureOpenDialog",
        "Open a texture",
        "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*",
    };

    // auto widget1 = std::make_unique<MyWidget>();
    auto widget2 = std::make_unique<Image>();

    auto widget3 = std::make_unique<FileDialog>(info);
    RegisterWidgets({widget2.get()});
    widget2->SetViewport(viewport.get());

    renderLayer->AddViewport(std::move(viewport));

    // drawLayer->AddWidget(std::move(widget1));
    drawLayer->AddWidget(std::move(widget2));
    drawLayer->AddWidget(std::move(widget3));

    dockspaceLayer->AddNextLayer(std::move(drawLayer));
    imguiLayer->AddNextLayer(std::move(dockspaceLayer));
    renderLayer->AddNextLayer(std::move(imguiLayer));

    firstLayer = std::move(renderLayer);
    firstLayer->Attach();

    auto _renderLayer = dynamic_cast<RenderLayer*>(GetLayer("RenderLayer"));
    auto image = dynamic_cast<Image*>(GetWidget("Image"));
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

Widget* Window::GetWidget(const String& widgetName) const {
    auto& widgets = windowData->widgetMap;
    if(widgets.find(widgetName) == widgets.end()) return nullptr;

    return widgets.at(widgetName);
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
