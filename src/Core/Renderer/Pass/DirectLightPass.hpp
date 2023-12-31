#pragma once

#include "Common/MathCommon.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"
#include "Core/Scene/Component/Component.hpp"
#include "Core/Scene/Scene.hpp"

#ifndef MAX_DIRECTION_LIGHT_COUNT
#define MAX_DIRECTION_LIGHT_COUNT 32
#endif

namespace Marbas {

struct DirectLightPassCreateInfo {
  RHIFactory* rhiFactory;
  uint32_t width;
  uint32_t height;
  RenderGraphTextureHandler diffuseTexture;
  RenderGraphTextureHandler normalTeture;
  RenderGraphTextureHandler positionTeture;
  RenderGraphTextureHandler roughnessTeture;
  RenderGraphTextureHandler metallicTeture;
  RenderGraphTextureHandler aoTeture;
  RenderGraphTextureHandler directionalShadowmap;
  RenderGraphTextureHandler indirectDiffuse;
  RenderGraphTextureHandler indirectSpecular;
  RenderGraphTextureHandler finalColorTexture;
};

class DirectLightPass {
 public:
  DirectLightPass(const DirectLightPassCreateInfo& createInfo);
  ~DirectLightPass();

 public:
  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList);

  bool
  IsEnable(RenderGraphGraphicsRegistry& registry);

 private:
  RHIFactory* m_rhiFactory;
  uint32_t m_width;
  uint32_t m_height;

  uintptr_t m_sampler;
  DescriptorSetArgument m_argument;
  uintptr_t m_descriptorSet;

  struct CameraInfo {
    glm::vec3 cameraPos;
    alignas(16) glm::mat4 cameraView;
  } m_cameraInfo;
  Buffer* m_cameraInfoBuffer = nullptr;

  RenderGraphTextureHandler m_diffuseTexture;
  RenderGraphTextureHandler m_normalTexture;
  RenderGraphTextureHandler m_positionTexture;
  RenderGraphTextureHandler m_roughnessTexture;
  RenderGraphTextureHandler m_metallicTexture;
  RenderGraphTextureHandler m_aoTexture;
  RenderGraphTextureHandler m_directionalShadowmap;
  RenderGraphTextureHandler m_finalColorTexture;
  RenderGraphTextureHandler m_indirectDiffuse;
  RenderGraphTextureHandler m_indirectSpecular;
};

}  // namespace Marbas
