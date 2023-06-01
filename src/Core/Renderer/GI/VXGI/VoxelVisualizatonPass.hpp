#pragma once

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"

namespace Marbas::GI {

struct VoxelVisulzationPassCreateInfo {
  RHIFactory* rhiFactory;
  uint32_t m_width;
  uint32_t m_height;
  RenderGraphTextureHandler m_voxelTexture;
  RenderGraphTextureHandler m_resultTexture;
  RenderGraphTextureHandler m_depthTexture;
};

class VoxelVisulzationPrepass {
 public:
  VoxelVisulzationPrepass();

 public:
  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList);

 private:
  RenderGraphTextureHandler m_frontTexture;
  RenderGraphTextureHandler m_backTexture;
};

class VoxelVisulzationPass {
 public:
  VoxelVisulzationPass(const VoxelVisulzationPassCreateInfo& createInfo);

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
  struct VoxelVisulizationInfo {
    glm::vec2 voxelResolutionSize = glm::vec2(256, 3000 / 256.0);
    glm::vec3 voxelCenter = glm::vec3(0, 0, 0);
  } m_voxelInfo;
  Buffer* m_voxelInfoBuffer = nullptr;

  struct CameraInfo {
    glm::mat4 view;
    glm::mat4 projection;
  } m_cameraInfo;
  Buffer* m_cameraInfoBuffer = nullptr;

  RenderGraphTextureHandler m_voxelTexture;
  RenderGraphTextureHandler m_resultTexture;
  RenderGraphTextureHandler m_depthTexture;
  uintptr_t m_sampler;

  DescriptorSetArgument m_argument;
  uintptr_t m_set;

  RHIFactory* m_rhiFactory;
  uint32_t m_width;
  uint32_t m_height;
};

}  // namespace Marbas::GI

namespace Marbas::Editor {

using VoxelVisulzationPass = GI::VoxelVisulzationPass;

};
