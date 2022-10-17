#pragma once

#include <memory>

#include "Common/Common.hpp"
#include "Common/MathCommon.hpp"
#include "entt/entity/entity.hpp"

namespace Marbas {

class Scene;
struct HierarchyComponent {
  entt::entity next = entt::null;
  entt::entity prew = entt::null;
  entt::entity parent = entt::null;
  Vector<entt::entity> children;

  glm::mat4 localTransformMatrix = glm::mat4(1.0);
  glm::mat4 globalTransformMatrix = glm::mat4(1.0);

  static void
  AddChild(const entt::entity parent, const std::shared_ptr<Scene>& scene, entt::entity child);

  static void
  AddChild(const entt::entity parent, entt::registry& registry, entt::entity child);
};

}  // namespace Marbas
