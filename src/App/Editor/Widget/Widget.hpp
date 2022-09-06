#pragma once

#include "Common/Common.hpp"
#include "Core/Scene/Entity/MeshEntity.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

class Widget {
 public:
  explicit Widget(const String& widgetName, RHIFactory* rhiFactory);
  Widget(const Widget&) = default;
  virtual ~Widget();

 public:
  [[nodiscard]] const String&
  GetWidgetName() const noexcept {
    return m_widgetName;
  }

  virtual void
  Draw() = 0;

  void
  SetScene(const std::shared_ptr<Scene>& scene) noexcept {
    m_scene = scene;
  }

  void
  SetResourceManager(const std::shared_ptr<ResourceManager>& resourceManager) noexcept {
    m_resourceManager = resourceManager;
  }

 protected:
  const String m_widgetName;
  std::shared_ptr<Scene> m_scene;
  std::shared_ptr<ResourceManager> m_resourceManager;
  RHIFactory* m_rhiFactory;
};

}  // namespace Marbas
