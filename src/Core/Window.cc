#include "Core/Window.h"
#include "Core/Application.h"
#include "Event/MouseEvent.h"
#include "Event/KeyEvent.h"
#include "Layer/ImguiLayer.h"
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

static void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                          GLsizei length, const char *message, const void *userParam) {

    // ignore these non-significant error codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

void Window::CreateSingleWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

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

    // enable OpenGL debug context if context allows for debug context
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        LOG(INFO) << "enable debug";
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    else {
        LOG(INFO) << "disable debug";
    }

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
