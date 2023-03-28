#pragma once

#include "Core/Renderer/RenderGraph/RenderCommandList.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

struct GridRenderPassCreateInfo {
  uint32_t width;
  uint32_t height;
  Scene* scene;
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
  Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList);

 private:
  struct CameraInfo {
    glm::mat4 view;
    glm::mat4 perspective;
    alignas(16) glm::vec3 right;
    alignas(16) glm::vec3 up;
    alignas(16) glm::vec3 pos;
    float FAR;
    float NEAR;
  } m_cameraInfo;
  Buffer* m_cameraInfoUBO;

  uint32_t m_width;
  uint32_t m_height;
  Scene* m_scene;
  RHIFactory* m_rhiFactory;

  RenderGraphTextureHandler m_finalColorTexture;
  RenderGraphTextureHandler m_finalDepthTexture;
};

}  // namespace Marbas
