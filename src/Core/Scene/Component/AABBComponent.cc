#include "AABBComponent.hpp"

#include "AssetManager/Mesh.hpp"

namespace Marbas {

AABBComponent::AABBComponent(const ModelAsset& model) {
  size_t meshCount = model.GetMeshCount();

  glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
  glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
  for (size_t i = 0; i < meshCount; i++) {
    const auto& mesh = model.GetMesh(i);
    for (const auto& vertex : mesh.m_vertices) {
      minAABB.x = std::min(vertex.posX, minAABB.x);
      minAABB.y = std::min(minAABB.y, vertex.posY);
      minAABB.z = std::min(minAABB.z, vertex.posZ);

      maxAABB.x = std::max(maxAABB.x, vertex.posX);
      maxAABB.y = std::max(maxAABB.y, vertex.posY);
      maxAABB.z = std::max(maxAABB.z, vertex.posZ);
    }
  }

  m_center = (maxAABB + minAABB) * 0.5f;
  m_halfExtent = glm::vec3(maxAABB.x - m_center.x, maxAABB.y - m_center.y, maxAABB.z - m_center.z);
}

bool
AABBComponent::IsOnFrustum(const Frustum& frustum, const glm::mat4& tranMatrix) const {
  // Get global scale thanks to our transform
  const glm::vec3 globalCenter{tranMatrix * glm::vec4(m_center, 1.f)};

  // Scaled orientation
  auto up = glm::normalize(glm::vec3(glm::column(glm::inverse(tranMatrix), 1))) * m_halfExtent.y;
  auto right = glm::normalize(glm::vec3(glm::column(glm::inverse(tranMatrix), 0))) * m_halfExtent.x;
  auto forward = -glm::normalize(glm::vec3(glm::column(glm::inverse(tranMatrix), 2))) * m_halfExtent.z;

  const float newIi = std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, right)) +
                      std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, up)) +
                      std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, forward));

  const float newIj = std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, right)) +
                      std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, up)) +
                      std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, forward));

  const float newIk = std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, right)) +
                      std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, up)) +
                      std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, forward));

  // We not need to divise scale because it's based on the half extention of the AABB
  const AABBComponent globalAABB(globalCenter, newIi, newIj, newIk);

  return (globalAABB.isOnOrForwardPlan(frustum.leftFace) && globalAABB.isOnOrForwardPlan(frustum.rightFace) &&
          globalAABB.isOnOrForwardPlan(frustum.topFace) && globalAABB.isOnOrForwardPlan(frustum.bottomFace) &&
          globalAABB.isOnOrForwardPlan(frustum.nearFace) && globalAABB.isOnOrForwardPlan(frustum.farFace));
}

}  // namespace Marbas
