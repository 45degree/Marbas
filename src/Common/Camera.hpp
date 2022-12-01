#pragma once

#include "Common/MathCommon.hpp"

namespace Marbas {

class Mesh;
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
  GetFrontVector() const {
    return glm::cross(GetRightVector(), GetUpVector());
  }

  virtual glm::vec3
  GetPosition() const = 0;

  virtual float
  GetFar() const = 0;

  virtual float
  GetNear() const = 0;

  virtual bool
  IsMeshVisible(const Mesh& mesh, const glm::mat4& transform) = 0;
};

}  // namespace Marbas
