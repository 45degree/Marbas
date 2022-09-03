#pragma once

#include "entt/entt.hpp"

namespace Marbas {

using LightEntity = entt::entity;

class Scene;

enum class LightType {
  ParalleLight,
  PointLight,
};

class LightPolicy {
 public:
  LightPolicy() = delete;
  LightPolicy(const LightPolicy&) = delete;
  LightPolicy&
  operator=(const LightPolicy&) = delete;

  static LightEntity
  Create(entt::registry& registry, LightType lightType);
};

}  // namespace Marbas
