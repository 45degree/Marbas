#pragma once

#include <imgui.h>

#include "Widget.hpp"

namespace Marbas {

class RenderImage : public Widget {
 public:
  explicit RenderImage(RHIFactory* rhiFactory, const String& title = "Image") : Widget(title, rhiFactory) {}

  ~RenderImage() override = default;

  void
  Draw() override;

  [[nodiscard]] const ImVec2
  GetImageSize() const noexcept {
    return imageSize;
  }

  void
  SetSelectedEntity(entt::entity entity) {
    m_entity = entity;
  }

  void
  SetRenderImage(ImageView* imageView) {
    m_renderImage = m_rhiFactory->GetImguiContext()->CreateImGuiImage(imageView);
  }

  bool needBakeScene = false;  // TODO: need remove

 private:
  void
  ShowToolBar(Scene* scene);

  void
  DrawModelManipulate(Scene* scene);

  void
  DrawLightManipulate(Scene* scene);

  void
  Manipulate(Scene* scene);

 private:
  ImTextureID m_renderImage = nullptr;

  ImVec2 imageSize;
  bool m_showMove = true;
  bool m_showRotate = true;
  bool m_showScale = true;
  entt::entity m_entity = entt::null;
};

}  // namespace Marbas
