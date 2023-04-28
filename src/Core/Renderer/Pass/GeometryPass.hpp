#pragma once

#include "AssetManager/AssetManager.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

struct GeometryPassCreateInfo {
  uint32_t width;
  uint32_t height;
  RenderGraphTextureHandler normalMetallicTexture;
  RenderGraphTextureHandler colorTexture;
  RenderGraphTextureHandler positionRoughnessTexture;
  RenderGraphTextureHandler depthTexture;
  RHIFactory* rhiFactory;
  Scene* scene;  // TODO: or passed when execute?
};

class GeometryPass {
  struct CameraMatrix {
    glm::mat4 view;
    glm::mat4 projection;
    alignas(16) glm::vec3 right;
    alignas(16) glm::vec3 up;
    alignas(16) glm::vec3 pos;
  } m_cameraMatrix;

 public:
  GeometryPass(const GeometryPassCreateInfo& createInfo);

 public:
  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphRegistry& registry, GraphicsCommandBuffer& commandList);

 private:
  RenderGraphTextureHandler m_normalMetallicTexture;
  RenderGraphTextureHandler m_colorTexture;
  RenderGraphTextureHandler m_positionRoughnessTexture;
  RenderGraphTextureHandler m_depthTexture;
  Scene* m_scene = nullptr;
  RHIFactory* m_rhiFactory = nullptr;

  DescriptorSetArgument m_argument;
  uintptr_t m_descriptorSet;

  uint32_t m_height = 0;
  uint32_t m_width = 0;

  uintptr_t m_sampler;
  Buffer* m_cameraBuffer = nullptr;
  Image* m_emptyImage = nullptr;
  ImageView* m_emptyImageView = nullptr;
};

}  // namespace Marbas
