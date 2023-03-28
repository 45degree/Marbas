#pragma once

#include <entt/entt.hpp>

#include "Common/Camera.hpp"
#include "Common/MathCommon.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphResource.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

/**
 * all direction shadow map store in a 2d texture array,
 * this component only store the subresource of the image
 */
struct DirectionShadowComponent {
  std::array<float, 4> split = {1 / 50.0f, 1 / 25.0f, 1 / 10.f, 1 / 2.0f};
  std::array<glm::mat4, 5> lightSpaceMatrices;
  glm::mat4 cameraViewMatrix;
  std::array<float, 5> cascadePlaneDistances;
  float farPlane;
  glm::vec3 lightDir;

  /**
   * @brief calculate light space matrices for all directional light
   *
   * @param world
   */
  static void
  Update(entt::registry& world, Camera* camera);
};

}  // namespace Marbas
