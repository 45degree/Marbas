#pragma once

#include "Common/Common.hpp"
#include "Core/Event/Event.hpp"
#include "Core/Event/KeyEvent.hpp"
#include "Core/Event/MouseEvent.hpp"
#include "Resource/ResourceManager.hpp"

namespace Marbas {

class Window;
class LayerBase {
 public:
  explicit LayerBase(const std::weak_ptr<Window>& window);

  virtual ~LayerBase();

 public:
  void
  AddNextLayer(const std::shared_ptr<LayerBase>& nextLayer) {
    m_nextLayer = nextLayer;
  }

 public:
  void
  Attach();

  void
  Detach();

  void
  Update();

  void
  Begin();

  void
  End();

  void
  BroadcastEvent(const Event&);

 public:
  virtual void
  OnAttach(){};

  virtual void
  OnDetach(){};

  virtual void
  OnUpdate(){};

  virtual void
  OnBegin(){};

  virtual void
  OnEnd(){};

  void
  EventDistribution(const Event& event);

 public:
  virtual void
  OnMousePress(const MousePressEvent& e) {}

  virtual void
  OnMouseRelease(const MouseReleaseEvent& e) {}

  virtual void
  OnKeyPress(const KeyEvent& e) {}

  virtual void
  OnKeyRelase(const KeyEvent& e) {}

  virtual void
  OnMouseMove(const MouseMoveEvent& e){};

  virtual void
  OnMouseScrolled(const MouseScrolledEvent& e){};

 protected:
  const std::weak_ptr<Window> m_window;
  std::shared_ptr<ResourceManager> m_resourceManager = nullptr;
  RHIFactory* m_rhiFactory = nullptr;

  std::shared_ptr<LayerBase> m_nextLayer = nullptr;
};

}  // namespace Marbas
