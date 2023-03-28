#pragma once

#include <imgui.h>

#include "Widget.hpp"

namespace Marbas {

class RenderImage : public Widget {
 public:
  explicit RenderImage(RHIFactory* rhiFactory, Scene* scene, const String& title = "Image")
      : Widget(title, rhiFactory), m_scene(scene) {}

  ~RenderImage() override = default;

  void
  Draw() override;

  [[nodiscard]] const ImVec2
  GetImageSize() const noexcept {
    return imageSize;
  }

  void
  SetScene(Scene* scene) {
    m_scene = scene;
  }

  void
  SetSelectedEntity(entt::entity entity) {
    m_entity = entity;
  }

  void
  SetRenderImage(ImageView* imageView) {
    // if (m_renderImage != nullptr) {
    //   m_rhiFactory->GetImguiContext()->DestroyImGuiImage(m_renderImage);
    // }
    m_renderImage = m_rhiFactory->GetImguiContext()->CreateImGuiImage(imageView);
  }

  bool needBakeScene = false;  // TODO: need remove

 private:
  void
  ShowToolBar();

  void
  DrawModelManipulate();

  void
  DrawLightManipulate();

  void
  Manipulate();

 private:
  ImTextureID m_renderImage = nullptr;
  Scene* m_scene;

  ImVec2 imageSize;
  bool m_showMove = true;
  bool m_showRotate = true;
  bool m_showScale = true;
  entt::entity m_entity = entt::null;
};

}  // namespace Marbas
