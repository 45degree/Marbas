#pragma once

#include <cereal/types/vector.hpp>
#include <memory>

#include "Common/Common.hpp"
#include "entt/entity/entity.hpp"

namespace Marbas {

struct HierarchyComponent {
  entt::entity next = entt::null;
  entt::entity prew = entt::null;
  entt::entity parent = entt::null;
  std::vector<entt::entity> children;

  static void
  AddChild(const entt::entity parent, entt::registry& registry, entt::entity child);

  template <typename Archive>
  void
  serialize(Archive&& archive) {
    archive(next, prew, parent, children);
  }
};

}  // namespace Marbas
