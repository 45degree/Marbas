#pragma once

#include "AssetManager/AssetManager.hpp"
#include "Core/Renderer/RenderGraph/RenderCommandList.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

struct GeometryPassCreateInfo {
  uint32_t width;
  uint32_t height;
  RenderGraphTextureHandler normalTexture;
  RenderGraphTextureHandler colorTexture;
  RenderGraphTextureHandler positionTexture;
  RenderGraphTextureHandler aoTexture;
  RenderGraphTextureHandler metallicTexture;
  RenderGraphTextureHandler roughnessTexture;
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
  Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList);

 private:
  RenderGraphTextureHandler m_normalTexture;
  RenderGraphTextureHandler m_colorTexture;
  RenderGraphTextureHandler m_positionTexture;
  RenderGraphTextureHandler m_aoTexture;
  RenderGraphTextureHandler m_metallicTexture;
  RenderGraphTextureHandler m_roughnessTexture;
  RenderGraphTextureHandler m_depthTexture;
  Scene* m_scene = nullptr;
  RHIFactory* m_rhiFactory = nullptr;

  uint32_t m_height = 0;
  uint32_t m_width = 0;

  Sampler* m_sampler = nullptr;
  Buffer* m_cameraBuffer = nullptr;
  Image* m_emptyImage = nullptr;
  ImageView* m_emptyImageView = nullptr;
};

}  // namespace Marbas
