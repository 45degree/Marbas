#include "Common/EditorCamera.hpp"

namespace Marbas {

EditorCamera::EditorCamera() : Camera() {
  auto pos = glm::normalize(glm::vec3(-1, 1, 1)) * m_distance;
  m_viewMatrix = glm::lookAt(pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  m_frustum = Frustum::CreateFrustumFromCamera(*this, m_fov, m_aspect);
}

EditorCamera::~EditorCamera() = default;

// bool
// EditorCamera::IsMeshVisible(const Mesh& mesh, const glm::mat4& transform) {
//   return mesh.m_volume->IsOnFrustum(m_frustum, transform);
// }

}  // namespace Marbas
