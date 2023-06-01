#pragma once

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"

namespace Marbas::GI {

struct LightInjectPassCreateInfo {
  RHIFactory* rhiFactory;
  RenderGraphTextureHandler voxelTexture;
  RenderGraphTextureHandler voxelNormal;
  RenderGraphTextureHandler voxelRadiance;
};

class LightInjectPass final {
 public:
  LightInjectPass(const LightInjectPassCreateInfo& createInfo);
  ~LightInjectPass();

 public:
  void
  SetUp(RenderGraphComputeBuilder& builder);

  void
  Execute(RenderGraphComputeRegistry& registry, ComputeCommandBuffer& commandBuffer);

 private:
  RHIFactory* m_rhiFactory = nullptr;
  struct VoxelInfo {
    glm::vec4 voxelResolutionSize = glm::vec4(256, 3000 / 256.0, 3000 / 256.0, 3000 / 256.0);
    glm::vec3 voxelCenter = glm::vec3(0, 0, 0);
  } m_voxelInfo;
  Buffer* m_voxelInfoBuffer;

  uintptr_t m_sampler;
  RenderGraphTextureHandler m_voxelTexture;
  RenderGraphTextureHandler m_voxelNormal;
  RenderGraphTextureHandler m_voxelRadiance;

  DescriptorSetArgument m_argument;
  uintptr_t m_set;
};

}  // namespace Marbas::GI
