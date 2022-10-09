#include "Core/Window.hpp"

#include <glog/logging.h>

#include <unordered_map>

#include "Core/Application.hpp"
#include "Core/Layer/DockspaceLayer.hpp"
#include "Core/Layer/ImguiLayer.hpp"
#include "Core/Layer/RenderLayer.hpp"
#include "Event/KeyEvent.hpp"
#include "Event/MouseEvent.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

struct WindowData {
  uint32_t height;
  uint32_t width;
  std::unique_ptr<EventCollection> eventCollection = nullptr;
  std::shared_ptr<LayerBase> customLayer = nullptr;

  std::shared_ptr<DockspaceLayer> dockerSpaceLayer = nullptr;
  std::shared_ptr<ImguiLayer> imguiLayer = nullptr;
  std::shared_ptr<RenderLayer> renderLayer = nullptr;
};

Window::Window(const WindowProp& winProp)
    : m_windowName(winProp.name), windowData(std::make_unique<WindowData>()) {
  m_rhiFactory = Application::GetRendererFactory();
  m_resourceManager = std::make_shared<ResourceManager>();
  windowData->width = winProp.width;
  windowData->height = winProp.height;
  windowData->eventCollection = std::make_unique<EventCollection>();
}

Window::~Window() {
  m_firstLayer->Detach();

  glfwTerminate();
}

void
Window::SetCustomLayer(const std::shared_ptr<LayerBase>& customLayer) {
  windowData->customLayer = customLayer;
}

void
Window::InitializeWindow() {
  int width = static_cast<int>(windowData->width);
  int height = static_cast<int>(windowData->height);
  glfwWindow = glfwCreateWindow(width, height, m_windowName.c_str(), nullptr, nullptr);

  if (glfwWindow == nullptr) {
    throw std::runtime_error("failed to create window");
  }

  m_rhiFactory->SetGLFWwindow(glfwWindow);
  m_rhiFactory->Init(RHICreateInfo{
      .m_openglRHICreateInfo = OpenGLRHICreateInfo{.useSPIRV = false},
  });

  m_swapChain = m_rhiFactory->GetSwapChain();
  auto imageCount = m_swapChain->GetImageCount();
  for (int i = 0; i < imageCount; i++) {
    m_aviableSemaphore.push_back(m_rhiFactory->CreateSemaphore());
    m_renderFinishSemaphore.push_back(m_rhiFactory->CreateSemaphore());
  }

  /**
   * setup callback function
   */
  SetUpEventCallBackFun();

  /**
   * set layer chain
   */
  windowData->renderLayer = std::make_shared<RenderLayer>(1920, 1080, shared_from_this());
  windowData->imguiLayer = std::make_shared<ImguiLayer>(shared_from_this());
  windowData->renderLayer->AddNextLayer(windowData->imguiLayer);
  windowData->dockerSpaceLayer = std::make_shared<DockspaceLayer>(shared_from_this());
  windowData->imguiLayer->AddNextLayer(windowData->dockerSpaceLayer);
  std::shared_ptr<LayerBase> lastLayer = windowData->dockerSpaceLayer;

  if (windowData->customLayer) {
    lastLayer->AddNextLayer(windowData->customLayer);
  }

  // windowData->resourceLayer->AddNextLayer(windowData->renderLayer);
  m_firstLayer = windowData->renderLayer;
  LOG(INFO) << "setup toolchain successful";

  glfwSetWindowUserPointer(glfwWindow, windowData.get());
  LOG(INFO) << "windows initialized successful";
}

void
Window::ShowWindow() {
  // push all event to every layer
  windowData->eventCollection->BroadcastEventFromLayer(m_firstLayer.get());
  windowData->eventCollection->ClearEvent();

  if (m_needResize) {
    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);

    if (width > 0 && height > 0) {
      m_swapChain->Resize(width, height);
      m_firstLayer->Resize(width, height);
      m_frameIndex = 0;
      m_needResize = false;
      return;
    }
  }

  int w, h;
  glfwGetWindowSize(glfwWindow, &w, &h);
  const bool is_minimized = (w <= 0.0f || h <= 0.0f);
  if (!is_minimized) {
    auto imageIndex = m_swapChain->AcquireNextImage(m_aviableSemaphore[m_frameIndex]);
    if (imageIndex == -1) {
      m_needResize = true;
      return;
    }

    // TODO: add semaphore(wait semaphore and finish semaphore)
    GlobalLayerInfo info{
        .globalStartSemaphore = m_aviableSemaphore[m_frameIndex],
        .gloablEndSemaphore = m_renderFinishSemaphore[m_frameIndex],
        .swapChianImageIndex = static_cast<uint32_t>(imageIndex),
    };
    m_firstLayer->Begin(info);
    m_firstLayer->Update(info);
    m_firstLayer->End(info);

    // imguiInterface->RenderData(avaliableImageSemaphore[currentFrame],
    //                            renderFinishSemaphore[currentFrame], imageIndex);

    if (m_swapChain->Present({m_renderFinishSemaphore[m_frameIndex]}, imageIndex) == -1) {
      m_needResize = true;
      return;
    }
  }
}

std::shared_ptr<RenderLayer>
Window::GetRenderLayer() const {
  return windowData->renderLayer;
}

std::shared_ptr<ImguiLayer>
Window::GetImGuiLayer() const {
  return windowData->imguiLayer;
}

void
Window::SetUpEventCallBackFun() {
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

  LOG(INFO) << "setup callback function successful";
}

std::tuple<uint32_t, uint32_t>
Window::GetWindowsSize() const {
  return std::make_tuple(windowData->width, windowData->height);
}

}  // namespace Marbas
