#pragma once

#include <memory>

#include "Common/Common.hpp"
#include "entt/entity/entity.hpp"

namespace Marbas {

class Scene;
struct HierarchyComponent {
  entt::entity next = entt::null;
  entt::entity prew = entt::null;
  entt::entity parent = entt::null;
  Vector<entt::entity> children;

  static void
  AddChild(const entt::entity parent, const std::shared_ptr<Scene>& scene, entt::entity child);

  static void
  AddChild(const entt::entity parent, entt::registry& registry, entt::entity child);
};

}  // namespace Marbas
