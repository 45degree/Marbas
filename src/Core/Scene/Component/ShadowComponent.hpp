#pragma once

#include <entt/entt.hpp>

#include "Common/Camera.hpp"
#include "Common/Light.hpp"
#include "Common/MathCommon.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphResource.hpp"
#include "RHIFactory.hpp"

#ifndef CASCATE_COUNT
#define CASCATE_COUNT 3
#endif

namespace Marbas {

/**
 * all direction shadow map store in a 2d texture array,
 * this component only store the subresource of the image
 */
struct DirectionShadowComponent {
  constexpr static int splitCount = CASCATE_COUNT;
  constexpr static int shadowMapArraySize = CASCATE_COUNT + 1;
  constexpr static uint32_t MAX_SHADOWMAP_SIZE = 4096;

  std::array<float, splitCount> m_split = {0.1, 0.2, 0.5};
  std::array<glm::mat4, splitCount + 1> m_lightSpaceMatrices;
  std::array<float, splitCount + 1> m_cascadePlane;
  glm::vec4 m_viewport;

 public:
  DirectionShadowComponent();

  void
  UpdateShadowInfo(const glm::vec3& lightDir, const Camera& camera);

 public:
  static void
  OnUpdate(entt::registry& world, entt::entity node);

  static void
  OnCreate(entt::registry& world, entt::entity node);

  static void
  OnDestroy(entt::registry& world, entt::entity node);
};

}  // namespace Marbas
