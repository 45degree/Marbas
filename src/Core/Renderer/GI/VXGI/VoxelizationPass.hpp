#pragma once

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"

namespace Marbas::GI {

class VoxelizationPass {
 public:
  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphRegistry& registry, GraphicsCommandBuffer& commandList);

  bool
  IsEnable(RenderGraphRegistry& registry);

 private:
  RenderGraphTextureHandler m_shadowMap;
  uintptr_t m_sampler;
};

}  // namespace Marbas::GI
