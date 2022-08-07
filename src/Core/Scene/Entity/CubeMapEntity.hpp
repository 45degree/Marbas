#pragma once

#include "RHI/Interface/RHIFactory.hpp"
#include "Tool/Uid.hpp"
#include "entt/entity/fwd.hpp"

namespace Marbas {

using CubeMapEntity = entt::entity;

class Scene;
class CubeMapPolicy {
 public:
  CubeMapPolicy() = delete;
  CubeMapPolicy(const CubeMapPolicy&) = delete;
  CubeMapPolicy&
  operator=(const CubeMapPolicy&) = delete;

  static CubeMapEntity
  Create(entt::registry& registry, Uid cubeMapResourceId);
};

}  // namespace Marbas
