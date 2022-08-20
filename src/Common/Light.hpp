#pragma once

#include "Common/MathCommon.hpp"

namespace Marbas {

class Light {
 public:
  Light() = default;
  ~Light() = default;

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
  GetColor() {
    return m_color;
  }

 private:
  glm::vec3 m_pos;
  glm::vec3 m_color;
};

}  // namespace Marbas
