#pragma once

#include "Widget.hpp"

namespace Marbas {

class InformationWidget : public Widget {
 public:
  explicit InformationWidget(RHIFactory* rhiFactory);
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
};

}  // namespace Marbas
