#pragma once

#include "Common/MathCommon.hpp"

namespace Marbas {

class Light {
 public:
  Light() = default;
  virtual ~Light() = default;

 public:
  void
  SetPos(const glm::vec3& pos) {
    m_pos = pos;
  }

  glm::vec3
  GetPos() const {
    return m_pos;
  }

  void
  SetColor(const glm::vec3& color) {
    m_color = color;
  }

  glm::vec3
  GetColor() const {
    return m_color;
  }

 protected:
  glm::vec3 m_pos;
  glm::vec3 m_color;
};

class ParallelLight final : public Light {
 public:
  ParallelLight() = default;
  ~ParallelLight() = default;

 public:
  glm::mat4
  GetProjectionMatrix() const {
    return glm::ortho(-10.0f, 10.0f, -100.0f, 100.0f, m_near, m_far);
  }

  glm::mat4
  GetViewMatrix() const {
    return glm::lookAt(m_pos, m_pos + m_direction, glm::vec3(0, 1, 0));
  }

  void
  SetNearPlane(float near) {
    m_near = near;
  }

  void
  SetFarPlane(float far) {
    m_far = far;
  }

  void
  SetDirection(const glm::vec3& direction) {
    m_direction = direction;
  }

 private:
  glm::vec3 m_direction = glm::vec3(0, 0, -1);
  float m_near = 0.1f;
  float m_far = 100.f;
};

}  // namespace Marbas
