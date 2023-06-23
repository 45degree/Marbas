#include "Common/Frustum.hpp"

namespace Marbas {

constexpr static float PI = 3.1415926535;

Frustum
Frustum::CreateFrustumFromCamera(const Camera& camera, float fov, float aspect) {
  auto near = camera.GetNear();
  auto far = camera.GetFar();
  auto pos = camera.GetPosition();
  auto frontVec = camera.GetFrontVector();
  auto upVec = camera.GetUpVector();
  auto rightVec = camera.GetRightVector();
  const auto halfV = far * std::tan(PI * fov / 180 * .5f);
  const auto halfH = halfV * aspect;

  Frustum frustum;
  frustum.nearFace = Plan(pos + frontVec * near, frontVec);
  frustum.farFace = Plan(pos + frontVec * far, -frontVec);
  frustum.bottomFace = Plan(pos, glm::cross(frontVec * far + halfV * upVec, rightVec));
  frustum.topFace = Plan(pos, glm::cross(rightVec, frontVec * far - halfV * upVec));
  frustum.rightFace = Plan(pos, glm::cross(frontVec * far - halfH * rightVec, upVec));
  frustum.leftFace = Plan(pos, glm::cross(upVec, frontVec * far + halfH * rightVec));

  return frustum;
}

}  // namespace Marbas
