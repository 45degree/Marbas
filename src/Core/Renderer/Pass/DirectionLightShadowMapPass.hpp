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

  Buffer* m_currentIndexBuf = nullptr;
  int m_currentLightIndex = 0;

  DescriptorSetArgument m_argument;
  uintptr_t m_set;

  RenderGraphTextureHandler m_shadowMapTextureHandler;
};

}  // namespace Marbas
