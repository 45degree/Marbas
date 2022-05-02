#include "Layer/LayerBase.hpp"

#include "Core/Window.hpp"

namespace Marbas {

LayerBase::LayerBase(const Window* window) : m_window(window) {}

LayerBase::~LayerBase() = default;

void LayerBase::Attach() {
  OnAttach();

  if (m_nextLayer != nullptr) {
    m_nextLayer->Attach();
  }
}

void LayerBase::Detach() {
  if (m_nextLayer != nullptr) {
    m_nextLayer->Detach();
  }
  OnDetach();
}

void LayerBase::Begin() {
  OnBegin();

  if (m_nextLayer != nullptr) {
    m_nextLayer->Begin();
  }
}

void LayerBase::End() {
  if (m_nextLayer != nullptr) {
    m_nextLayer->End();
  }

  OnEnd();
}

void LayerBase::Update() {
  OnUpdate();

  if (m_nextLayer != nullptr) {
    m_nextLayer->Update();
  }
}

void LayerBase::BroadcastEvent(const Event& e) {
  EventDistribution(e);

  if (m_nextLayer != nullptr) {
    m_nextLayer->BroadcastEvent(e);
  }
}

void LayerBase::EventDistribution(const Event& event) {
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
