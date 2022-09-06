#pragma once

#include <imgui.h>

#include "App/Editor/Widget/Widget.hpp"
#include "Core/Layer/RenderLayer.hpp"
#include "Core/Scene/Component/ModelComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"
#include "Core/Scene/Entity/ModelEntity.hpp"

namespace Marbas {

class RenderImage : public Widget {
 public:
  explicit RenderImage(RHIFactory* rhiFactory, const String& title = "Image")
      : Widget(title, rhiFactory) {}

  ~RenderImage() override = default;

  void
  Draw() override;

  [[nodiscard]] const ImVec2
  GetImageSize() const noexcept {
    return imageSize;
  }

  void
  SetSelectedModel(entt::entity modelEntity) {
    if (!Entity::HasComponent<UniqueTagComponent>(m_scene.get(), modelEntity)) return;

    const auto& tagComp = Entity::GetComponent<UniqueTagComponent>(m_scene.get(), modelEntity);
    auto type = tagComp.type;
    if (type == EntityType::Model) {
      m_modelEntity = modelEntity;
    }
  }

 private:
  void
  ShowToolBar();

 private:
  ImVec2 imageSize;
  bool m_showMove = true;
  bool m_showRotate = true;
  bool m_showScale = true;
  std::optional<ModelEntity> m_modelEntity = std::nullopt;
};

}  // namespace Marbas
