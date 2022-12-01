#include "Common/Frustum.hpp"

namespace Marbas {

Frustum
Frustum::CreateFrustumFromCamera(const Camera &camera, float fov, float aspect) {
  auto near = camera.GetNear();
  auto far = camera.GetFar();
  auto pos = camera.GetPosition();
  auto frontVec = camera.GetFrontVector();
  auto upVec = camera.GetUpVector();
  auto rightVec = camera.GetRightVector();
  const auto halfV = far * std::tan(M_PIf * fov / 180 * .5f);
  const auto halfH = halfV * aspect;

  Frustum frustum;
  frustum.nearFace = Plan(camera.GetPosition() + frontVec * near, frontVec);
  frustum.farFace = Plan(camera.GetPosition() + frontVec * far, -frontVec);
  frustum.bottomFace = Plan(pos, -glm::cross(frontVec * far - halfV * upVec, rightVec));
  frustum.topFace = Plan(pos, glm::cross(frontVec * far + halfV * upVec, rightVec));
  frustum.rightFace = Plan(pos, -glm::cross(frontVec * far + halfH * rightVec, upVec));
  frustum.leftFace = Plan(pos, glm::cross(frontVec * far - halfH * rightVec, upVec));

  return frustum;
}

}  // namespace Marbas
