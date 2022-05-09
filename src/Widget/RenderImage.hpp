#ifndef MARBARS_WIDGET_IMAGE_H
#define MARBARS_WIDGET_IMAGE_H

#include <imgui.h>

#include "Layer/RenderLayer.hpp"
#include "Widget/Widget.hpp"

namespace Marbas {

class RenderImage : public Widget, public IChangeMeshAble {
 public:
  explicit RenderImage(const String& title = "Image") : Widget(title) {}

  ~RenderImage() override = default;

  void Draw() override;

  [[nodiscard]] const ImVec2 GetImageSize() const noexcept { return imageSize; }

  void ChangeMesh(const Mesh& mesh) override { m_selectedMesh = mesh; }

 private:
  std::optional<Mesh> m_selectedMesh;
  ImVec2 imageSize;
};

}  // namespace Marbas

#endif
