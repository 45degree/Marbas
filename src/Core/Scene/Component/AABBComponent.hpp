#pragma once

#include "AssetManager/ModelAsset.hpp"
#include "Common/Frustum.hpp"
#include "Common/MathCommon.hpp"

namespace Marbas {

struct AABBComponent final {
 public:
  AABBComponent() = default;
  ~AABBComponent() = default;

  AABBComponent(const glm::vec3& min, const glm::vec3& max)
      : m_center((min + max) * 0.5f), m_halfExtent{max.x - m_center.x, max.y - m_center.y, max.z - m_center.z} {}

  AABBComponent(const glm::vec3& inCenter, float iI, float iJ, float iK)
      : m_center{inCenter}, m_halfExtent{iI, iJ, iK} {}

  AABBComponent(const ModelAsset& model);

  AABBComponent(const AABBComponent& obj) : m_center(obj.m_center), m_halfExtent(obj.m_halfExtent) {}

  AABBComponent&
  operator=(const AABBComponent& obj) {
    m_center = obj.m_center;
    m_halfExtent = obj.m_halfExtent;
    return *this;
  }

 public:
  glm::vec3
  GetCenter() const {
    return m_center;
  }

  glm::vec3
  GetExtent() const {
    return m_halfExtent + m_halfExtent;
  }

  bool
  IsOnFrustum(const Frustum& frustum, const glm::mat4& tranMatrix) const;

  bool
  isOnOrForwardPlan(const Plan& plan) const {
    // @see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
    const float r = m_halfExtent.x * std::abs(plan.normal.x) + m_halfExtent.y * std::abs(plan.normal.y) +
                    m_halfExtent.z * std::abs(plan.normal.z);

    bool res = -r <= plan.getSignedDistanceToPlan(m_center);
    return res;
  }

 private:
  glm::vec3 m_center;
  glm::vec3 m_halfExtent;  // (x, y, z) / 2
};

}  // namespace Marbas
