#ifndef MARBARS_CORE_CAMERA_H
#define MARBARS_CORE_CAMERA_H

#include "MathCommon.hpp"

/**
 * @brief this camera always looks at a fixed point, and can rotate around it
 */
class Camera {
 public:
  Camera() = default;
  ~Camera() = default;

 public:
  [[nodiscard]] glm::vec3 GetFixPoint() const noexcept { return m_fixPoint; }

  [[nodiscard]] glm::mat4 GetViewMartix() const noexcept {
    auto direction = glm::vec3(0, 0, 0);
    direction.x = std::cos(glm::radians(m_pitch)) * std::sin(glm::radians(m_yaw));
    direction.y = std::sin(glm::radians(m_pitch));
    direction.z = std::cos(glm::radians(m_pitch)) * std::cos(glm::radians(m_yaw));

    return glm::lookAt(m_fixPoint + distance * direction, m_fixPoint, m_upDirection);
  }

  [[nodiscard]] glm::mat4 GetPerspective() const noexcept {
    return glm::perspective(glm::radians(fov), m_aspect, m_near, m_far);
  }

  void SetFixPoint(const glm::vec3& newPos) { m_fixPoint = newPos; }

  void SetAspect(float aspect) noexcept { m_aspect = aspect; }

  void AddPitch(float pitch) noexcept {
    m_pitch += pitch;
    if (m_pitch > 89.0) {
      m_pitch = 89.0f;
    } else if (m_pitch < -89.0) {
      m_pitch = -89.0f;
    }
  }

  void AddYaw(float yaw) noexcept { m_yaw += yaw; }

  void MoveFixPoint(float xOffset, float yOffset) noexcept {
    m_fixPoint.x += xOffset * 0.1f;
    m_fixPoint.z += yOffset * 0.1f;
  }

  void AddFov(float fovOffset) noexcept {
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

 private:
  float m_near = 0.1f;
  float m_far = 100.0f;
  float distance = 50.f;
  float fov = 45.0f;
  float m_aspect = 800.f / 600.f;
  glm::vec3 m_fixPoint = glm::vec3(0, 0, 0);
  glm::vec3 m_upDirection = glm::vec3(0, 1, 0);

  float m_pitch = 40.0f;
  float m_yaw = 0.0f;
};

#endif
