#pragma once

#include "Common/MathCommon.hpp"

namespace Marbas {

class Camera {
 public:
  virtual glm::mat4
  GetViewMatrix() const = 0;

  virtual glm::mat4
  GetProjectionMatrix() const = 0;

  virtual glm::vec3
  GetUpVector() const = 0;

  virtual glm::vec3
  GetRightVector() const = 0;

  virtual glm::vec3
  GetPosition() const = 0;
};

}  // namespace Marbas
