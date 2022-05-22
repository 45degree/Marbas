#pragma once

#include <imgui.h>

#include "App/Editor/Widget/Widget.hpp"
#include "Core/Layer/RenderLayer.hpp"
#include "Core/Scene/Component/ModelComponent.hpp"
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
  SetSelectedModel(ModelEntity modelEntity, const ModelComponent&) {
    m_modelEntity = modelEntity;
  }

 private:
  ImVec2 imageSize;
  std::optional<ModelEntity> m_modelEntity = std::nullopt;
};

}  // namespace Marbas
