#pragma once

#include "Common/Common.hpp"
#include "Common/MathCommon.hpp"

namespace Marbas {

class Mesh;
class MARBAS_EXPORT Camera {
 public:
  glm::mat4
  GetViewMatrix() const {
    return m_viewMatrix;
  }

  glm::mat4
  GetProjectionMatrix() const {
    auto proj = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
    proj[1][1] *= -1;
    return proj;
  }

  glm::vec3
  GetUpVector() const {
    return glm::normalize(glm::vec3(glm::column(glm::inverse(m_viewMatrix), 1)));
  }

  glm::vec3
  GetRightVector() const {
    return glm::normalize(glm::vec3(glm::column(glm::inverse(m_viewMatrix), 0)));
  }

  glm::vec3
  GetFrontVector() const {
    return -glm::normalize(glm::vec3(glm::column(glm::inverse(m_viewMatrix), 2)));
  }

  glm::vec3
  GetPosition() const {
    return glm::vec3(glm::column(glm::inverse(m_viewMatrix), 3));
  }

  float
  GetFar() const {
    return m_far;
  }

  float
  GetNear() const {
    return m_near;
  }

  float
  GetAspect() const {
    return m_aspect;
  }

  float
  GetFov() const {
    return m_fov;
  }

  glm::vec3
  GetLookAtVector() const noexcept {
    return -glm::normalize(glm::vec3(glm::column(glm::inverse(m_viewMatrix), 2)));
  }

  // virtual bool
  // IsMeshVisible(const Mesh& mesh, const glm::mat4& transform) = 0;

 protected:
  float m_near = 0.1f;
  float m_far = 10000.f;
  float m_fov = 45.0f;
  float m_aspect = 800.f / 600.f;
  glm::mat4 m_viewMatrix;

  constexpr static float MaxDistance = 10000.f;
  constexpr static float MinDistance = 1.f;
};

}  // namespace Marbas
