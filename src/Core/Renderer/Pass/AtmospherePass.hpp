#pragma once

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

struct AtmospherePassCreateInfo {
  Scene* scene;
  RHIFactory* rhiFactory;
  uint32_t height;
  uint32_t width;
  RenderGraphTextureHandler multiscatterLUT;
  RenderGraphTextureHandler transmittanceLUT;
  RenderGraphTextureHandler colorTexture;
};

class AtmospherePass final {
 public:
  AtmospherePass(const AtmospherePassCreateInfo& createInfo);
  ~AtmospherePass();

 public:
  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphRegistry& registry, GraphicsCommandBuffer& commandList);

  bool
  IsEnable();

 private:
  Scene* m_scene = nullptr;
  RHIFactory* m_rhiFactory = nullptr;

  struct CameraInfo {
    glm::mat4 view;
    glm::mat4 projection;
  } m_cameraInfo;
  Buffer* m_cameraInfoUBO = nullptr;

  struct AtmosphereInfo {
    // the start point is the sun, and it has beed normalized
    glm::vec3 lightDir = -glm::normalize(glm::vec3(0, 0.1, -1));
    alignas(16) glm::vec3 lightColor = glm::vec3(1, 1, 1);
    float sunLuminace = 30;

    // Atmosphere Parap
    float atmosphereHeight = 60000;
    float rayleighScalarHeight = 8500;
    float mieScalarHeight = 1200;
    float mieAnisotropy = 0.5;  // [-1, 1]
    float planetRadius = 6360000;
    float ozoneCenterHeight = 25000;
    float ozoneWidth = 15000;
  } m_atmosphereInfo;
  Buffer* m_atmosphereInfoBuffer = nullptr;
  uintptr_t m_sampler;

  DescriptorSetArgument m_argument;
  uintptr_t m_descriptorSet;
  DescriptorSetArgument m_inputArgument;

  Buffer* m_vertexBuffer = nullptr;
  Buffer* m_indexBuffer = nullptr;

  uint32_t m_height;
  uint32_t m_width;

  RenderGraphTextureHandler m_transmittanceLUT;
  RenderGraphTextureHandler m_multiscatterLUT;
  RenderGraphTextureHandler m_finalColorTexture;
};

}  // namespace Marbas
