#pragma once

#include <entt/entt.hpp>

#include "Tool/Uid.hpp"

namespace Marbas {

using EmptySceneNodeEntity = entt::entity;

class EmptySceneNodeEntityPolicy {
 public:
  static EmptySceneNodeEntity
  Create(entt::registry& registry);
};

}  // namespace Marbas
