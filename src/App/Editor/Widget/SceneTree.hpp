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

  template <auto Candidate, typename Type>
  void
  Connect(Type&& instance) {
    m_sink.connect<Candidate, Type>(std::forward<Type>(instance));
  }

 private:
  void
  DrawPopup();

  void
  DrawNode(const entt::entity& rootEntity);

 private:
  entt::entity m_PopupEntity = entt::null;
  String m_popUpName = "";
  entt::sigh<void(entt::entity)> m_signal;
  entt::sink<decltype(m_signal)> m_sink{m_signal};
};

}  // namespace Marbas
