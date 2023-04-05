#pragma once

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

struct DirectionShadowMapPassCreateInfo {
  RHIFactory* rhiFactory;
  Scene* scene;
  RenderGraphTextureHandler directionalShadowMap;
};

class DirectionShadowMapPass final {
 public:
  DirectionShadowMapPass(const DirectionShadowMapPassCreateInfo& createInfo);

  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphRegistry& registry, GraphicsCommandBuffer& commandList);

  bool
  IsEnable();

 private:
  Scene* m_scene = nullptr;
  RHIFactory* m_rhiFactory = nullptr;

  DescriptorSetArgument m_argument;

  RenderGraphTextureHandler m_shadowMapTextureHandler;
  uint32_t m_width = 1024;
  uint32_t m_height = 1024;
};

}  // namespace Marbas
