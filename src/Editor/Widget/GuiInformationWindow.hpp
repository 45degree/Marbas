#pragma once

#include "Core/Scene/Scene.hpp"
#include "GuiWindow.hpp"
#include "RHIFactory.hpp"

namespace Marbas::Gui {

class GuiInformationWindow final : public GuiWindow {
 public:
  explicit GuiInformationWindow(RHIFactory* rhiFactory);
  ~GuiInformationWindow() override = default;

 public:
  void
  OnDraw() override;

 public:
  void
  SelectEntity(Scene& scene, entt::entity entity) {
    m_entity = entity;
  }

 private:
  entt::entity m_entity = entt::null;
  RHIFactory* m_rhiFactory = nullptr;
};

}  // namespace Marbas::Gui
