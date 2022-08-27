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
    return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, m_near, m_far);
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

 private:
  glm::vec3 m_direction;
  float m_near = 1.0f;
  float m_far = 7.5f;
};

}  // namespace Marbas
