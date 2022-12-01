#include "Common/EditorCamera.hpp"

#include "Common/Mesh.hpp"

namespace Marbas {

bool
EditorCamera::IsMeshVisible(const Mesh& mesh, const glm::mat4& transform) {
  return mesh.m_volume->IsOnFrustum(m_frustum, transform);
}

}  // namespace Marbas
