#pragma once

#include "Common/Camera.hpp"
#include "Common/MathCommon.hpp"

namespace Marbas {

struct Plan {
  glm::vec3 normal = {0, 1, 0};
  glm::vec3 point = {0, 0, 0};
};

struct Frustum {
  Plan topFace;
  Plan bottomFace;

  Plan rightFace;
  Plan leftFace;

  Plan farFace;
  Plan nearFace;

  static Frustum
  CreateFrustumFromCamera(const Camera& camera);
};

}  // namespace Marbas
