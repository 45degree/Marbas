#include "Core/Window.hpp"

#include <glog/logging.h>

#include <unordered_map>

#include "Core/Application.hpp"
#include "Event/KeyEvent.hpp"
#include "Event/MouseEvent.hpp"
#include "Layer/DockspaceLayer.hpp"
#include "Layer/DrawLayer.hpp"
#include "Layer/ImguiLayer.hpp"
#include "Layer/RenderLayer.hpp"
#include "Layer/ResourceLayer.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

struct WindowData {
  uint32_t height;
  uint32_t width;
  std::unique_ptr<EventCollection> eventCollection = nullptr;
  std::unique_ptr<DockspaceLayer> m_dockerSpaceLayer;
  std::unique_ptr<DrawLayer> m_drawLayer;
  std::unique_ptr<ImguiLayer> m_imguiLayer;
  std::unique_ptr<RenderLayer> m_renderLayer;
  std::unique_ptr<ResourceLayer> m_resourceLayer = nullptr;
};

Window::Window(const WindowProp& winProp)
    : m_windowName(winProp.name), windowData(std::make_unique<WindowData>()) {
  m_rhiFactory = Application::GetRendererFactory();
  windowData->width = winProp.width;
  windowData->height = winProp.height;
  windowData->eventCollection = std::make_unique<EventCollection>();
}

Window::~Window() {
  firstLayer->Detach();

  glfwTerminate();
  // LOG(INFO) << "destroy windows";
}

void Window::CreateSingleWindow() {
  m_rhiFactory = Application::GetRendererFactory();

  int width = static_cast<int>(windowData->width);
  int height = static_cast<int>(windowData->height);
  glfwWindow = glfwCreateWindow(width, height, m_windowName.c_str(), nullptr, nullptr);

  if (glfwWindow == nullptr) {
    throw std::runtime_error("failed to create window");
  }

  m_rhiFactory->Init(glfwWindow);
  m_rhiFactory->PrintRHIInfo();

  // setup callback function
  SetUpEventCallBackFun();

  // attach layer

  auto resourceManager = std::make_unique<ResourceManager>(m_rhiFactory);
  auto drawLayer = std::make_unique<DrawLayer>(this, resourceManager.get());
  auto renderLayer = std::make_unique<RenderLayer>(1920, 1080, resourceManager.get(), this);
  auto dockSpaceLayer = std::make_unique<DockspaceLayer>(this);
  auto imguiLayer = std::make_unique<ImguiLayer>(this);

  auto resourceLayer = std::make_unique<ResourceLayer>(std::move(resourceManager), this);

  dockSpaceLayer->AddNextLayer(drawLayer.get());
  imguiLayer->AddNextLayer(dockSpaceLayer.get());
  renderLayer->AddNextLayer(imguiLayer.get());
  resourceLayer->AddNextLayer(renderLayer.get());

  firstLayer = resourceLayer.get();

  windowData->m_imguiLayer = std::move(imguiLayer);
  windowData->m_dockerSpaceLayer = std::move(dockSpaceLayer);
  windowData->m_drawLayer = std::move(drawLayer);
  windowData->m_renderLayer = std::move(renderLayer);
  windowData->m_resourceLayer = std::move(resourceLayer);

  firstLayer->Attach();
  glfwSetWindowUserPointer(glfwWindow, windowData.get());
}

void Window::ShowWindow() {
  // push all event to every layer
  windowData->eventCollection->BroadcastEventFromLayer(firstLayer);
  windowData->eventCollection->ClearEvent();

  firstLayer->Begin();
  firstLayer->Update();
  firstLayer->End();

  // swap buffer
  m_rhiFactory->SwapBuffer(glfwWindow);

  // clear buffer
  m_rhiFactory->ClearBuffer(ClearBuferBit::COLOR_BUFFER);
}

RenderLayer* Window::GetRenderLayer() const { return windowData->m_renderLayer.get(); }

DrawLayer* Window::GetDrawLayer() const { return windowData->m_drawLayer.get(); }

ImguiLayer* Window::GetImGuiLayer() const { return windowData->m_imguiLayer.get(); }

void Window::SetUpEventCallBackFun() {
  glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* glfwWindow, double xpos, double ypos) {
    auto windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(glfwWindow));
    auto event = std::make_unique<MouseMoveEvent>();
    event->SetPos({xpos, ypos});
    windowData->eventCollection->AddEvent(std::move(event));
  });

  glfwSetMouseButtonCallback(
      glfwWindow, [](GLFWwindow* glfwWindow, int button, int action, int mods) {
        auto windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(glfwWindow));

        double xpos, ypos;
        glfwGetCursorPos(glfwWindow, &xpos, &ypos);
        if (action == GLFW_PRESS) {
          auto event = std::make_unique<MousePressEvent>(button);
          event->SetPos({xpos, ypos});
          windowData->eventCollection->AddEvent(std::move(event));
        } else if (action == GLFW_RELEASE) {
          auto event = std::make_unique<MouseReleaseEvent>(button);
          event->SetPos({xpos, ypos});
          windowData->eventCollection->AddEvent(std::move(event));
        }
        GLFWkeyfun a;
      });

  glfwSetKeyCallback(
      glfwWindow, [](GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
        auto windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(glfwWindow));
        if (action == GLFW_PRESS) {
          auto event = std::make_unique<KeyEvent>(EventType::MARBAS_KEY_PRESS_EVENT);
          windowData->eventCollection->AddEvent(std::move(event));
        } else if (action == GLFW_RELEASE) {
          auto event = std::make_unique<KeyEvent>(EventType::MARBAS_KEY_RELEASE_EVENT);
          windowData->eventCollection->AddEvent(std::move(event));
        }
      });

  glfwSetScrollCallback(glfwWindow, [](GLFWwindow* glfwWindow, double xOffset, double yOffset) {
    auto windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(glfwWindow));

    double xpos, ypos;
    glfwGetCursorPos(glfwWindow, &xpos, &ypos);

    auto event = std::make_unique<MouseScrolledEvent>(xOffset, yOffset);
    event->SetPos({xpos, ypos});

    windowData->eventCollection->AddEvent(std::move(event));
  });

  glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* glfwWindow, int width, int height) {
    auto windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(glfwWindow));
    windowData->width = width;
    windowData->height = height;
  });
}

std::tuple<uint32_t, uint32_t> Window::GetWindowsSize() const {
  return std::make_tuple(windowData->width, windowData->height);
}

}  // namespace Marbas
