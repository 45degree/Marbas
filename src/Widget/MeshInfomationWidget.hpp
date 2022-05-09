#ifndef MARBAS_WIDGET_MESH_INFOMATION_WIDGET_HPP
#define MARBAS_WIDGET_MESH_INFOMATION_WIDGET_HPP

#include "Widget/Widget.hpp"

namespace Marbas {

class MeshInfomationWidget : public Widget, public IChangeMeshAble {
 public:
  MeshInfomationWidget() : Widget("MeshInfomation") {}
  ~MeshInfomationWidget() override = default;

 public:
  void Draw() override;

  void ChangeMesh(const Mesh& mesh) override { m_selectedMesh = mesh; }

 private:
  std::optional<Mesh> m_selectedMesh;
};

}  // namespace Marbas

#endif
