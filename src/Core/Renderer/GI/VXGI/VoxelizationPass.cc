#include "VoxelizationPass.hpp"

namespace Marbas::GI {

void
VoxelizationPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.ReadTexture(m_shadowMap, m_sampler);

  builder.SetFramebufferSize(2, 2, 0);
}

void
VoxelizationPass::Execute(RenderGraphRegistry& registry, GraphicsCommandBuffer& commandBuffer) {}

};  // namespace Marbas::GI
