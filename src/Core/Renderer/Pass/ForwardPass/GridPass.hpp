#pragma once

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"
#include "Core/Scene/Scene.hpp"
#include "Core/Scene/System/RenderSystemJob/RenderSystem.hpp"

namespace Marbas {

struct GridRenderPassCreateInfo {
  uint32_t width;
  uint32_t height;
  // Scene* scene;
  RHIFactory* rhiFactory;
  RenderGraphTextureHandler finalColorTexture;
  RenderGraphTextureHandler finalDepthTexture;
};

class GridRenderPass final {
 public:
  GridRenderPass(const GridRenderPassCreateInfo& createInfo);
  ~GridRenderPass();

  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList);

  bool
  IsEnable(RenderGraphGraphicsRegistry& registry) {
    auto* userData = reinterpret_cast<Job::RenderUserData*>(registry.GetUserData());
    auto* scene = userData->scene;
    return scene != nullptr;
  }

 private:
  struct CameraInfo {
    glm::mat4 view;
    glm::mat4 perspective;
    alignas(16) glm::vec3 right;
    alignas(16) glm::vec3 up;
    alignas(16) glm::vec3 pos;
    float m_far;
    float m_near;
  } m_cameraInfo;
  Buffer* m_cameraInfoUBO;

  DescriptorSetArgument m_argument;
  uintptr_t m_descriptorSet;

  uint32_t m_width;
  uint32_t m_height;
  // Scene* m_scene;
  RHIFactory* m_rhiFactory;

  RenderGraphTextureHandler m_finalColorTexture;
  RenderGraphTextureHandler m_finalDepthTexture;
};

}  // namespace Marbas
