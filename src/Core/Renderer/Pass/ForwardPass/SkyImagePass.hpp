#pragma once

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

struct SkyImagePassCreateInfo {
  uint32_t width;
  uint32_t height;
  RHIFactory* rhiFactory = nullptr;
  RenderGraphTextureHandler finalColorTexture;
  RenderGraphTextureHandler finalDepthTexture;
  RenderGraphTextureHandler atmosphereTexture;
};

class SkyImagePass final {
 public:
  SkyImagePass(const SkyImagePassCreateInfo& createInfo);
  ~SkyImagePass();

  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList);

  bool
  IsEnable(RenderGraphGraphicsRegistry& registry);

 private:
  struct CameraInfo {
    glm::mat4 view;
    glm::mat4 projection;
  } m_cameraInfo;

  struct ClearValueInfo {
    glm::uint isClear;
    alignas(16) glm::vec4 clearValue;
  } m_clearInfo;

  DescriptorSetArgument m_atmosphereArgument;
  DescriptorSetArgument m_argument;
  uintptr_t m_descriptorSet;

  Buffer* m_cameraInfoUBO = nullptr;
  Buffer* m_clearUBO = nullptr;
  Buffer* m_vertexBuffer = nullptr;
  Buffer* m_indexBuffer = nullptr;
  uintptr_t m_sampler;

  RHIFactory* m_rhiFactory = nullptr;
  uint32_t m_width;
  uint32_t m_height;
  RenderGraphTextureHandler m_finalColorTexture;
  RenderGraphTextureHandler m_finalDepthTexture;
  RenderGraphTextureHandler m_atmosphereTexture;
};

}  // namespace Marbas
