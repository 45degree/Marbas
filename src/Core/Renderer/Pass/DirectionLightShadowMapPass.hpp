#pragma once

#include "Core/Renderer/RenderGraph/RenderCommandList.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

struct DirectShadowMapData {
  RenderGraphTextureHandler shadowMapTextureHandler;
};

class DirectionShadowMapPass final {
 public:
  DirectionShadowMapPass(RHIFactory* rhiFactory, Scene* scene, entt::entity light, RenderGraphTextureHandler shadowMap)
      : m_rhiFactory(rhiFactory), m_scene(scene), m_light(light), m_shadowMapTextureHandler(shadowMap) {
    auto* bufCtx = m_rhiFactory->GetBufferContext();
    m_lightShadowInfoBuffer =
        bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_lightShadowInfo, sizeof(LightShadowInfo), false);
  }

  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList);

 private:
  Scene* m_scene = nullptr;
  entt::entity m_light = entt::null;
  RHIFactory* m_rhiFactory = nullptr;

  struct LightShadowInfo {
    glm::mat4 lightSpaceMatrices[5];
    size_t lightSpaceMatricesCount;
  } m_lightShadowInfo;
  Buffer* m_lightShadowInfoBuffer = nullptr;

  RenderGraphTextureHandler m_shadowMapTextureHandler;
  uint32_t m_width = 1024;
  uint32_t m_height = 1024;
};

}  // namespace Marbas
