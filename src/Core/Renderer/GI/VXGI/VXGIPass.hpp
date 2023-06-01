#pragma once

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"

namespace Marbas::GI {

struct VXGIPassCreateInfo {
  RHIFactory* m_rhiFactory;
  RenderGraphTextureHandler m_voxelTexture;
  RenderGraphTextureHandler m_positionRoughnessTexture;
  RenderGraphTextureHandler m_normalMetallicTexture;
  RenderGraphTextureHandler m_diffuseTexture;
  RenderGraphTextureHandler m_finalTexture;
  uint32_t m_widht;
  uint32_t m_height;
};

class VXGIPass {
 public:
  VXGIPass(const VXGIPassCreateInfo& createInfo);
  ~VXGIPass() = default;

  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList);

 private:
  uint32_t m_width;
  uint32_t m_height;

  struct VoxelInfo {
    glm::vec4 voxelResolutionSize = glm::vec4(256, 256, 256, 3000 / 256.0);
    glm::vec3 voxelCenter = glm::vec3(0, 0, 0);
  } m_voxelInfo;

  struct CameraInfo {
    glm::vec3 cameraPos = glm::vec3(0, 0, 0);
  } m_cameraInfo;

  DescriptorSetArgument m_argument;
  uintptr_t m_set;
  Buffer* m_voxelInfoBuffer;
  Buffer* m_cameraInfoBuffer;

  RHIFactory* m_rhiFactory;
  uintptr_t m_sampler;
  RenderGraphTextureHandler m_voxelTexture;
  RenderGraphTextureHandler m_positionRoughnessTexture;
  RenderGraphTextureHandler m_normalMetallicTexture;
  RenderGraphTextureHandler m_diffuseTexture;
  RenderGraphTextureHandler m_finalTexture;
};

}  // namespace Marbas::GI
