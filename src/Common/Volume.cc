#include "Common/Volume.hpp"

#include "Common/Mesh.hpp"

namespace Marbas {

bool
AABB::IsOnFrustum(const Frustum &frustum, const glm::mat4 &tranMatrix) {
  // Get global scale thanks to our transform
  const glm::vec3 globalCenter{tranMatrix * glm::vec4(m_center, 1.f)};

  // Scaled orientation
  auto up = glm::normalize(glm::vec3(glm::column(glm::inverse(tranMatrix), 1))) * m_extent.y;
  auto right = glm::normalize(glm::vec3(glm::column(glm::inverse(tranMatrix), 0))) * m_extent.x;
  auto forward = -glm::normalize(glm::vec3(glm::column(glm::inverse(tranMatrix), 2))) * m_extent.z;

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
  const AABB globalAABB(globalCenter, newIi, newIj, newIk);

  return (globalAABB.isOnOrForwardPlan(frustum.leftFace) &&
          globalAABB.isOnOrForwardPlan(frustum.rightFace) &&
          globalAABB.isOnOrForwardPlan(frustum.topFace) &&
          globalAABB.isOnOrForwardPlan(frustum.bottomFace) &&
          globalAABB.isOnOrForwardPlan(frustum.nearFace) &&
          globalAABB.isOnOrForwardPlan(frustum.farFace));
}

AABB
AABB::GenerateAABB(const Mesh &mesh) {
  glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
  glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
  for (const auto &vertex : mesh.m_vertices) {
    minAABB.x = std::min(vertex.posX, minAABB.x);
    minAABB.y = std::min(minAABB.y, vertex.posY);
    minAABB.z = std::min(minAABB.z, vertex.posZ);

    maxAABB.x = std::max(maxAABB.x, vertex.posX);
    maxAABB.y = std::max(maxAABB.y, vertex.posY);
    maxAABB.z = std::max(maxAABB.z, vertex.posZ);
  }

  return AABB(minAABB, maxAABB);
}

}  // namespace Marbas
