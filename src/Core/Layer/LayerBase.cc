#include "Core/Layer/LayerBase.hpp"

#include "Core/Window.hpp"

namespace Marbas {

LayerBase::LayerBase(const std::weak_ptr<Window>& window) : m_window(window) {
  if (m_window.expired()) {
    throw std::runtime_error("the window's pointer is expired.");
  }
  auto windowPtr = m_window.lock();
  m_resourceManager = windowPtr->GetResourceManager();
  m_rhiFactory = windowPtr->GetRHIFactory();
}

LayerBase::~LayerBase() = default;

void
LayerBase::Attach() {
  OnAttach();

  if (m_nextLayer != nullptr) {
    m_nextLayer->Attach();
  }
}

void
LayerBase::Detach() {
  if (m_nextLayer != nullptr) {
    m_nextLayer->Detach();
  }
  OnDetach();
}

void
LayerBase::Begin(const GlobalLayerInfo& info) {
  OnBegin(info);

  if (m_nextLayer != nullptr) {
    m_nextLayer->Begin(info);
  }
}

void
LayerBase::End(const GlobalLayerInfo& info) {
  if (m_nextLayer != nullptr) {
    m_nextLayer->End(info);
  }

  OnEnd(info);
}

void
LayerBase::Resize(uint32_t width, uint32_t height) {
  OnResize(width, height);
  if (m_nextLayer != nullptr) {
    m_nextLayer->Resize(width, height);
  }
}

void
LayerBase::Update(const GlobalLayerInfo& info) {
  OnUpdate(info);

  if (m_nextLayer != nullptr) {
    m_nextLayer->Update(info);
  }
}

void
LayerBase::BroadcastEvent(const Event& e) {
  EventDistribution(e);

  if (m_nextLayer != nullptr) {
    m_nextLayer->BroadcastEvent(e);
  }
}

void
LayerBase::EventDistribution(const Event& event) {
  switch (event.GetEventType()) {
    case EventType::MARBAS_MOUSE_PRESS_EVENT:
      OnMousePress(static_cast<const MousePressEvent&>(event));
      break;
    case EventType::MARBAS_MOUSE_MOVE_EVENT:
      OnMouseMove(static_cast<const MouseMoveEvent&>(event));
      break;
    case EventType::MARBAS_MOUSE_RELEASE_EVENT:
      OnMouseRelease(static_cast<const MouseReleaseEvent&>(event));
      break;
    case EventType::MARBAS_MOUSE_SCROLLED_EVENT:
      OnMouseScrolled(static_cast<const MouseScrolledEvent&>(event));
      break;
    case EventType::MARBAS_KEY_PRESS_EVENT:
      OnKeyPress(static_cast<const KeyEvent&>(event));
      break;
    case EventType::MARBAS_KEY_RELEASE_EVENT:
      OnKeyRelase(static_cast<const KeyEvent&>(event));
      break;
  }
}

}  // namespace Marbas
