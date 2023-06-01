#pragma once

#include "Common/MathCommon.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphResource.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

struct SSAOCreateInfo {
  uint32_t width;
  uint32_t height;
  RHIFactory* rhiFactory;
  RenderGraphTextureHandler posTexture;
  RenderGraphTextureHandler normalTexture;
  RenderGraphTextureHandler depthTexture;
  RenderGraphTextureHandler ssaoTexture;
};

class SSAOPass {
 public:
  SSAOPass(const SSAOCreateInfo& createInfo);

  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList);

  bool
  IsEnable(RenderGraphGraphicsRegistry& registry) {
    return registry.GetCurrentActiveScene() != nullptr;
  }

 private:
  RHIFactory* m_rhiFactory;
  uintptr_t m_sampler;

  struct CameraInfo {
    alignas(4) float m_near;
    alignas(4) float m_far;
    alignas(16) glm::vec3 m_position;
    alignas(16) glm::mat4 m_viewMatrix;
    alignas(16) glm::mat4 m_projectMatrix;
  } m_cameraInfo;
  Buffer* m_cameraBuffer = nullptr;

  DescriptorSetArgument m_argument;
  uintptr_t m_set;

  uint32_t m_width;
  uint32_t m_height;
  RenderGraphTextureHandler m_posTexture;
  RenderGraphTextureHandler m_normalTexture;
  RenderGraphTextureHandler m_depthTexture;
  RenderGraphTextureHandler m_ssaoTexture;
};

}  // namespace Marbas
