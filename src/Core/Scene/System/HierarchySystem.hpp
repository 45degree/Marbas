#pragma once

#include <entt/entt.hpp>

#include "Common/MathCommon.hpp"

namespace Marbas {

class HierarchySystem final {
 public:
  static void
  UpdateTransformMatrix(entt::registry& registry);

  static void
  UpdateChildrenTransformMatrix(entt::registry& registry, entt::entity parent);

  static void
  ResetGlobalTransformMatrix(entt::registry& registry, entt::entity current,
                             const glm::mat4& globalTransformMatrix);
};

}  // namespace Marbas
