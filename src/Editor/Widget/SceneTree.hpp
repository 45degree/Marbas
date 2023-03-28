#pragma once

#include "Core/Scene/Scene.hpp"
#include "FileDialog.hpp"
#include "Widget.hpp"

namespace Marbas {

class SceneTreeWidget : public Widget {
 public:
  explicit SceneTreeWidget(RHIFactory* rhiFactory, Scene* scene);
  ~SceneTreeWidget() override = default;

 public:
  void
  Draw() override;

 public:
  Signal<void(entt::entity)> m_selectEntity;

 private:
  void
  DrawPopup();

  void
  DrawNode(const entt::entity& rootEntity, uint32_t indent = 0);

  void
  SetScene(Scene* scene) {
    m_scene = scene;
  }

  String
  AddIconForStringByTag(const char* str, entt::entity entity);

 private:
  Scene* m_scene;

  entt::entity m_PopupEntity = entt::null;
  String m_popUpName = "";
};

}  // namespace Marbas