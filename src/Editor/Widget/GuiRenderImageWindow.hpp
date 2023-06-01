#pragma once

#include <optional>

#include "./GuiWindow.hpp"
#include "Core/Scene/Scene.hpp"
#include "GuiComponent.hpp"

namespace Marbas::Gui {

struct ImguiZmoDrawInfo {
  bool showMove = false;
  bool showRotate = false;
  bool showScale = false;
};

class RenderImageWidget : public GuiWidget {
 public:
  RenderImageWidget(const std::string& name) : GuiWidget(name) {}
  virtual ~RenderImageWidget() = default;

 public:
  void
  SetImage(ImTextureID imageId) {
    m_image = imageId;
  }

  void
  OnDraw() override final;

 public:
  void
  SetGuizmoDrawInfo(const ImguiZmoDrawInfo& drawInfo) {
    m_zmoDrawInfo = drawInfo;
  }

  void
  SetGuizmoEntity(Scene& scene, entt::entity entity) {
    m_zmoEntity = entity;
  }

 private:
  void
  Manipulate(Scene* scene);

  void
  DrawModelManipulate(Scene* scene, entt::entity entity, Camera* camera);

  void
  DrawLightManipulate(Scene* scene, entt::entity entity, Camera* camera);

 private:
  std::optional<ImTextureID> m_image;
  ImguiZmoDrawInfo m_zmoDrawInfo;
  entt::entity m_zmoEntity = entt::null;
};

class RenderImageToolBar : public GuiWidget {
 public:
  RenderImageToolBar(const std::string& name) : GuiWidget(name) {}
  virtual ~RenderImageToolBar() = default;

 public:
  void
  OnDraw() override final;

  void
  SetFocusEntity(Scene& scene, entt::entity entity) {
    m_focusEntity = entity;
  }

 public:
  Signal<void(const ImguiZmoDrawInfo&)> ChangeZmoDrawInfoSign;

 private:
  entt::entity m_focusEntity = entt::null;
};

class RenderImageWindow : public GuiWindow {
 public:
  RenderImageWindow(const std::string& name) : GuiWindow(name), m_imageWidget(name), m_toolBar(name) {
    m_toolBar.ChangeZmoDrawInfoSign.Connect<&RenderImageWidget::SetGuizmoDrawInfo>(m_imageWidget);
  }
  RenderImageWindow(const std::string& name, ImTextureID imageId) : RenderImageWindow(name) {
    m_imageWidget.SetImage(imageId);
  }
  virtual ~RenderImageWindow() = default;

 public:
  void
  SetRHIFactory(RHIFactory* rhiFactory) {
    m_rhiFactory = rhiFactory;
  }

  void
  SetRenderImage(ImageView* imageView) {
    auto image = m_rhiFactory->GetImguiContext()->CreateImGuiImage(imageView);
    m_imageWidget.SetImage(image);
  }

 public:
  void
  OnSelectedEntity(Scene& scene, entt::entity entity);

 protected:
  void
  OnDraw() {
    m_toolBar.Draw();
    m_imageWidget.Draw();
  }

 private:
  RHIFactory* m_rhiFactory;
  RenderImageWidget m_imageWidget;
  RenderImageToolBar m_toolBar;
};

}  // namespace Marbas::Gui
