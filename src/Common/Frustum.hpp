#pragma once

#include "Common/Camera.hpp"
#include "Common/MathCommon.hpp"

namespace Marbas {

struct MARBAS_EXPORT Plan {
  Plan() = default;
  Plan(const glm::vec3& point, const glm::vec3& normal)
      : normal(glm::normalize(normal)), distance(glm::dot(point, glm::normalize(normal))) {}

  Plan(const Plan& plan) : normal(plan.normal), distance(plan.distance) {}

  Plan&
  operator=(const Plan& plan) {
    normal = plan.normal;
    distance = plan.distance;
    return *this;
  }

  ~Plan() = default;

  float
  getSignedDistanceToPlan(const glm::vec3& point) const {
    return glm::dot(normal, point) - distance;
  }

 public:
  glm::vec3 normal = glm::vec3(0, 1, 0);
  float distance = 0;
};

struct MARBAS_EXPORT Frustum {
  Plan topFace;
  Plan bottomFace;

  Plan rightFace;
  Plan leftFace;

  Plan farFace;
  Plan nearFace;

  static Frustum
  CreateFrustumFromCamera(const Camera& camera, float fov, float aspect);
};

}  // namespace Marbas
