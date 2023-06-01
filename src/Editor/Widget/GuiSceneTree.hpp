#pragma once

#include "Core/Scene/Scene.hpp"
#include "GuiWindow.hpp"

namespace Marbas::Gui {

class GuiSceneTree final : public GuiWindow {
 public:
  GuiSceneTree(const std::string& name) : GuiWindow(name){};
  ~GuiSceneTree() override = default;

 public:
  void
  SetScene(Scene* scene) {
    m_scene = scene;
  }

 public:
  Signal<void(Scene&, entt::entity)> SelectEntitySign;

 protected:
  void
  OnDraw() override;

  void
  DrawNode(entt::entity entity);

 private:
  Scene* m_scene = nullptr;
  entt::entity m_selectNode = entt::null;
};

};  // namespace Marbas::Gui
