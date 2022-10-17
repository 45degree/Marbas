#include "Common/Frustum.hpp"

namespace Marbas {

Frustum
Frustum::CreateFrustumFromCamera(const Camera &camera) {
  auto near = camera.GetNear();
  auto far = camera.GetFar();

  return Frustum();
}

}  // namespace Marbas
