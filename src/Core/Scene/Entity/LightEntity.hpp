#pragma once

#include "entt/entt.hpp"

namespace Marbas {

using LightEntity = entt::entity;

class Scene;
class LightPolicy {
 public:
  LightPolicy() = delete;
  LightPolicy(const LightPolicy&) = delete;
  LightPolicy&
  operator=(const LightPolicy&) = delete;

  static LightEntity
  Create(entt::registry& registry);
};

}  // namespace Marbas
