#pragma once

#include "Common/MathCommon.hpp"

/**
 * @brief this camera always looks at a fixed point, and can rotate around it
 */
class EditorCamera {
 public:
  EditorCamera() = default;
  ~EditorCamera() = default;

 public:
  [[nodiscard]] glm::mat4
  GetViewMartix() const noexcept {
    auto direction = glm::vec3(0, 0, 0);
    direction.x = std::cos(glm::radians(m_pitch)) * std::sin(glm::radians(m_yaw));
    direction.y = std::sin(glm::radians(m_pitch));
    direction.z = std::cos(glm::radians(m_pitch)) * std::cos(glm::radians(m_yaw));

    return glm::lookAt(m_fixPoint + m_distance * direction, m_fixPoint, m_upDirection);
  }

  [[nodiscard]] glm::vec3
  GetPosition() const noexcept {
    auto direction = glm::vec3(0, 0, 0);
    direction.x = std::cos(glm::radians(m_pitch)) * std::sin(glm::radians(m_yaw));
    direction.y = std::sin(glm::radians(m_pitch));
    direction.z = std::cos(glm::radians(m_pitch)) * std::cos(glm::radians(m_yaw));

    return m_fixPoint + direction * m_distance;
  }

  [[nodiscard]] glm::mat4
  GetPerspective() const noexcept {
    return glm::perspective(glm::radians(fov), m_aspect, m_near, m_far);
  }

  glm::vec3
  GetUpVector() const noexcept {
    auto lookatDirection = GetLookAtVector();
    return glm::cross(glm::cross(lookatDirection, glm::vec3(0, 1, 0)), lookatDirection);
  }

  glm::vec3
  GetRightVector() const noexcept {
    auto lookatDirection = GetLookAtVector();
    return glm::cross(lookatDirection, glm::vec3(0, 1, 0));
  }

  glm::vec3
  GetLookAtVector() const noexcept {
    auto direction = glm::vec3(0, 0, 0);
    direction.x = std::cos(glm::radians(m_pitch)) * std::sin(glm::radians(m_yaw));
    direction.y = std::sin(glm::radians(m_pitch));
    direction.z = std::cos(glm::radians(m_pitch)) * std::cos(glm::radians(m_yaw));

    return -direction;
  }

  void
  SetFixPoint(const glm::vec3& newPos) {
    m_fixPoint = newPos;
  }

  void
  SetAspect(float aspect) noexcept {
    m_aspect = aspect;
  }

  void
  AddPitch(float pitch) noexcept {
    m_pitch += pitch;
    if (m_pitch > 89.0) {
      m_pitch = 89.0f;
    } else if (m_pitch < -89.0) {
      m_pitch = -89.0f;
    }
  }

  void
  AddYaw(float yaw) noexcept {
    m_yaw += yaw;
  }

  void
  MoveFixPoint(float xOffset, float yOffset, float zOffset) noexcept {
    m_fixPoint.x += xOffset * 0.1f;
    m_fixPoint.y += yOffset * 0.1f;
    m_fixPoint.z += zOffset * 0.1f;
  }

  void
  AddFov(float fovOffset) noexcept {
    if (fov >= 1.0f && fov <= 45.0f) {
      fov -= fovOffset;
    }
    if (fov <= 1.0f) {
      fov = 1.0f;
    }
    if (fov >= 45.0f) {
      fov = 45.0f;
    }
  }

  void
  AddDistance(float distanceDiff) {
    m_distance += distanceDiff;
    if (m_distance > MaxDistance) m_distance = MaxDistance;
    if (m_distance < MinDistance) m_distance = MinDistance;
  }

 private:
  float m_near = 0.1f;
  float m_far = MaxDistance;
  float m_distance = 50.f;
  float fov = 45.0f;
  float m_aspect = 800.f / 600.f;
  glm::vec3 m_fixPoint = glm::vec3(0, 0, 0);
  glm::vec3 m_upDirection = glm::vec3(0, 1, 0);

  float m_pitch = 40.0f;
  float m_yaw = 0.0f;

  constexpr static float MaxDistance = 10000.f;
  constexpr static float MinDistance = 1.f;
};
