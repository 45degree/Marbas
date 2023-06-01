#pragma once

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"

namespace Marbas::GI {

struct VoxelizationCreateInfo {
  RenderGraphTextureHandler shadowMap;
  RenderGraphTextureHandler voxelScene;
  RHIFactory* rhiFactory;
};

struct VoxelInfo {
  glm::mat4 projX;
  glm::mat4 projY;
  glm::mat4 projZ;
  glm::mat4 viewMatrix;
  int voxelResolution = 256;
};

class VoxelizationPass {
 public:
  VoxelizationPass(const VoxelizationCreateInfo& createInfo);
  ~VoxelizationPass();

 public:
  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList);

  bool
  IsEnable(RenderGraphGraphicsRegistry& registry) {
    return true;
  }

 private:
  RenderGraphTextureHandler m_shadowMap;
  RenderGraphTextureHandler m_voxelScene;

  VoxelInfo m_voxelInfo;
  Buffer* m_voxelInfoBuffer;

  DescriptorSetArgument m_argument;
  uintptr_t m_set;

  RHIFactory* m_rhiFactory;
  uintptr_t m_sampler;
};

}  // namespace Marbas::GI
