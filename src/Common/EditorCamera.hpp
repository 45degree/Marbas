#pragma once

#include "Common/Camera.hpp"
#include "Common/Common.hpp"
#include "Common/Frustum.hpp"
#include "Common/MathCommon.hpp"
// #include "Common/Mesh.hpp"

/**
 * @brief this camera always looks at a fixed point, and can rotate around it
 */
namespace Marbas {

class MARBAS_EXPORT EditorCamera final : public Camera {
 public:
  EditorCamera();
  virtual ~EditorCamera();

 public:
  [[nodiscard]] float
  GetDistance() const noexcept {
    return m_distance;
  }

  void
  SetViewMatrix(const glm::mat4& viewMatrix) {
    m_viewMatrix = viewMatrix;
  }

  void
  SetAspect(float aspect) noexcept {
    m_aspect = aspect;
    m_frustum = Frustum::CreateFrustumFromCamera(*this, m_fov, m_aspect);
  }

  glm::vec3
  GetLookPosition() {
    return GetPosition() + GetLookAtVector() * m_distance;
  }

  void
  AddPitch(float pitch) noexcept {
    auto lookVec = -GetLookAtVector() * m_distance;
    float curPitch = glm::degrees(std::asin(lookVec.y / glm::length(lookVec)));
    if (curPitch + pitch > 89.0 || curPitch + pitch < -89.0) return;

    auto inverseMatrix = glm::inverse(m_viewMatrix);

    auto rotateMatrix = glm::rotate(glm::mat4(1.0), glm::radians(-pitch), GetRightVector());
    auto pos = GetLookPosition();
    auto transMatrix1 = glm::translate(glm::mat4(1.0), -pos);
    auto transMatrix2 = glm::translate(glm::mat4(1.0), pos);
    m_viewMatrix = glm::inverse(transMatrix2 * rotateMatrix * transMatrix1 * inverseMatrix);
  }

  void
  AddYaw(float yaw) noexcept {
    auto inverseMatrix = glm::inverse(m_viewMatrix);
    auto rotateMatrix = glm::rotate(glm::mat4(1.0), glm::radians(yaw), glm::vec3(0, 1, 0));
    auto transMatrix1 = glm::translate(glm::mat4(1.0), -GetLookPosition());
    auto transMatrix2 = glm::translate(glm::mat4(1.0), GetLookPosition());
    m_viewMatrix = glm::inverse(transMatrix2 * rotateMatrix * transMatrix1 * inverseMatrix);
  }

  void
  MoveFixPoint(float xOffset, float yOffset, float zOffset) noexcept {
    auto transMatrix = glm::translate(glm::mat4(1.0), glm::vec3(xOffset, yOffset, zOffset));
    m_viewMatrix = glm::inverse(transMatrix * glm::inverse(m_viewMatrix));
  }

  void
  MovePosition(const glm::vec3& pos) {
    auto currentPos = GetLookPosition();
    auto transMatrix = glm::translate(glm::mat4(1.0), pos - currentPos);
    m_viewMatrix = glm::inverse(transMatrix * glm::inverse(m_viewMatrix));
  }

  void
  AddFov(float fovOffset) noexcept {
    if (m_fov >= 1.0f && m_fov <= 45.0f) {
      m_fov -= fovOffset;
    }
    if (m_fov <= 1.0f) {
      m_fov = 1.0f;
    }
    if (m_fov >= 45.0f) {
      m_fov = 45.0f;
    }
    m_frustum = Frustum::CreateFrustumFromCamera(*this, m_fov, m_aspect);
  }

  void
  AddDistance(float distanceDiff) {
    if (m_distance + distanceDiff > MaxDistance || m_distance + distanceDiff < MinDistance) return;
    auto transMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, distanceDiff));
    m_distance += distanceDiff;
    m_viewMatrix = glm::inverse(glm::inverse(m_viewMatrix) * transMatrix);
    m_frustum = Frustum::CreateFrustumFromCamera(*this, m_fov, m_aspect);
  }

  const Frustum&
  GetFrustum() const {
    return m_frustum;
  }

 private:
  float m_distance = 50.0f;

  Frustum m_frustum;
};

}  // namespace Marbas
