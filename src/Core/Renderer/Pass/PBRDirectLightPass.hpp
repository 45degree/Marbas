#pragma once

#include <entt/entt.hpp>

#include "Core/Renderer/RenderGraph/RenderCommandList.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"

namespace Marbas {

class PBRDirectLightPass final {
 public:
  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList);

 private:
  RHIFactory* rhiFactory = nullptr;
  entt::entity m_light = entt::null;

  RenderGraphTextureHandler m_normalTexture;
  RenderGraphTextureHandler m_colorTexture;
  RenderGraphTextureHandler m_roughnessTexture;

  uint32_t m_width;
  uint32_t m_height;
};

}  // namespace Marbas
