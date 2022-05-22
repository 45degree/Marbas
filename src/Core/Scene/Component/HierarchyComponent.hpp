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
};

}  // namespace Marbas
