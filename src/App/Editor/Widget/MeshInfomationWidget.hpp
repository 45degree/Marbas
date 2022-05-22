#pragma once

#include "App/Editor/Widget/Widget.hpp"

namespace Marbas {

class MeshInfomationWidget : public Widget {
 public:
  explicit MeshInfomationWidget(RHIFactory* rhiFactory) : Widget("MeshInfomation", rhiFactory) {}
  ~MeshInfomationWidget() override = default;

 public:
  void
  Draw() override;

 private:
  std::optional<MeshEntity> m_selectedMesh;
};

}  // namespace Marbas
