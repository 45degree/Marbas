#pragma once

#include "App/Editor/Widget/FileDialog.hpp"
#include "App/Editor/Widget/Widget.hpp"
#include "Core/Scene/Component/ModelComponent.hpp"
#include "Core/Scene/Entity/ModelEntity.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

class SceneTreeWidget : public Widget {
 public:
  explicit SceneTreeWidget(RHIFactory* rhiFactory);
  ~SceneTreeWidget() override = default;

 public:
  void
  Draw() override;

  auto&
  GetSink() {
    return m_sink;
  }

 private:
  void
  DrawNode(const entt::entity& rootEntity);

 private:
  entt::sigh<void(ModelEntity, const ModelComponent&)> m_signal;
  entt::sink<decltype(m_signal)> m_sink{m_signal};
};

}  // namespace Marbas
