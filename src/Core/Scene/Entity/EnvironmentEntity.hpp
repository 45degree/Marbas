#pragma once

#include "RHI/Interface/RHIFactory.hpp"
#include "Tool/Uid.hpp"
#include "entt/entity/fwd.hpp"

namespace Marbas {

using EnvironmentEntity = entt::entity;

class Scene;
class EnvironmentPolicy {
 public:
  EnvironmentPolicy() = delete;
  EnvironmentPolicy(const EnvironmentPolicy&) = delete;
  EnvironmentPolicy&
  operator=(const EnvironmentPolicy&) = delete;

  static EnvironmentEntity
  Create(entt::registry& registry, const Path& path);
};

}  // namespace Marbas
