#pragma once

// #include "Information/LightInformation.hpp"
// #include "Information/ModelInformation.hpp"
#include "Widget.hpp"

namespace Marbas {

class InformationWidget : public Widget {
 public:
  explicit InformationWidget(RHIFactory* rhiFactory, Scene* scene);
  ~InformationWidget() override = default;

 public:
  void
  Draw() override;

  void
  SelectEntity(entt::entity entity) {
    m_entity = entity;
  }

  void
  SetScene(Scene* scene) {
    m_scene = scene;
  }

 private:
  entt::entity m_entity = entt::null;

  // ModelInformation m_modelInformation;
  // LightInformation m_lightInformation;

  Scene* m_scene;
};

}  // namespace Marbas
