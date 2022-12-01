#pragma once

#include <memory>

#include "Common/Camera.hpp"
#include "Common/Frustum.hpp"

namespace Marbas {

class Mesh;
class Volume {
 public:
  virtual bool
  IsOnFrustum(const Frustum& frustum, const glm::mat4& tranMatrix) = 0;
};

class AABB final : public Volume {
 public:
  AABB(const glm::vec3& min, const glm::vec3& max)
      : Volume(),
        m_center{(max + min) * 0.5f},
        m_extent{max.x - m_center.x, max.y - m_center.y, max.z - m_center.z} {}

  AABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
      : Volume{}, m_center{inCenter}, m_extent{iI, iJ, iK} {}

  ~AABB() = default;

 public:
  bool
  IsOnFrustum(const Frustum& frustum, const glm::mat4& tranMatrix) override;

  bool
  isOnOrForwardPlan(const Plan& plan) const {
    // @see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
    const float r = m_extent.x * std::abs(plan.normal.x) + m_extent.y * std::abs(plan.normal.y) +
                    m_extent.z * std::abs(plan.normal.z);

    bool res = -r <= plan.getSignedDistanceToPlan(m_center);
    return res;
  }

  glm::vec3
  GetCenter() const {
    return m_center;
  }

  glm::vec3
  GetExtent() const {
    return m_extent;
  }

 public:
  static AABB
  GenerateAABB(const Mesh& mesh);

 private:
  glm::vec3 m_center;
  glm::vec3 m_extent;
};

}  // namespace Marbas
