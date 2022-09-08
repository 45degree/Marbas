#pragma once

#include "App/Editor/Widget/Information/LightInformation.hpp"
#include "App/Editor/Widget/Information/ModelInformation.hpp"
#include "App/Editor/Widget/Widget.hpp"
#include "Core/Scene/Entity/Entity.hpp"

namespace Marbas {

class InformationWidget : public Widget {
 public:
  explicit InformationWidget(RHIFactory* rhiFactory)
      : Widget("Infomation", rhiFactory),
        m_modelInformation(std::make_unique<ModelInformation>()),
        m_lightInformation(std::make_unique<LightInformation>()) {}
  ~InformationWidget() override = default;

 public:
  void
  Draw() override;

  void
  SelectEntity(entt::entity entity) {
    m_entity = entity;
  }

 private:
  entt::entity m_entity = entt::null;

  std::unique_ptr<ModelInformation> m_modelInformation;
  std::unique_ptr<LightInformation> m_lightInformation;
};

}  // namespace Marbas
