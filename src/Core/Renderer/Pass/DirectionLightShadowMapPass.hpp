#pragma once

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"

namespace Marbas {

struct DirectionShadowMapPassCreateInfo {
  RHIFactory* rhiFactory;
  RenderGraphTextureHandler directionalShadowMap;
};

class DirectionShadowMapPass final {
 public:
  DirectionShadowMapPass(const DirectionShadowMapPassCreateInfo& createInfo);

  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList);

  bool
  IsEnable(RenderGraphGraphicsRegistry& registry);

 private:
  RHIFactory* m_rhiFactory = nullptr;
  DescriptorSetArgument m_argument;
  uintptr_t m_set;

  RenderGraphTextureHandler m_shadowMapTextureHandler;

  struct Constant {
    glm::mat4 model;
    int lightIndex;
  } m_constant;
};

}  // namespace Marbas
