#pragma once

#include <entt/entt.hpp>

#include "Common/MathCommon.hpp"
#include "Core/Renderer/RenderGraph/RenderGraph.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

/**
 * @class ScreenSpaceShadowInfoPass
 * @brief Get screen space shadow info from shadom map
 *
 */
class ScreenSpaceShadowInfoPass final {
 public:
  ScreenSpaceShadowInfoPass(entt::entity light, RHIFactory* rhiFactory, Scene* scene);
  ~ScreenSpaceShadowInfoPass();

  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList);

 private:
  uint32_t m_height = 1080;
  uint32_t m_width = 1920;

  RHIFactory* m_rhiFactory = nullptr;
  Scene* m_scene = nullptr;

  struct LightInfo {
    glm::mat4 m_lightMatrix[5];
    glm::mat4 cameraViewMatrix;
    float cascadePlaneDistances[5];
    float farPlane;
    glm::vec3 lightDir;
  } m_lightInfo;
  Buffer* m_lightInfoBuffer;
  Sampler* m_sampler;

  entt::entity m_light;
  RenderGraphTextureHandler m_positionTexture;
  RenderGraphTextureHandler m_shadowMapTexture;
  RenderGraphTextureHandler m_screenSpaceShadowInfoTexture;
};

}  // namespace Marbas
