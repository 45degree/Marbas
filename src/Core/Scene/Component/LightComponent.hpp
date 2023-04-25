#pragma once

#include <entt/entt.hpp>
#include <optional>

#include "Common/Light.hpp"
#include "TagComponent.hpp"

namespace Marbas {

struct DirectionLightComponent {
  std::optional<uint32_t> lightIndex;
  ParallelLight m_light;

  static void
  OnCreate(entt::registry& world, entt::entity node) {
    if (!world.any_of<NewLightTag>(node)) {
      world.emplace<NewLightTag>(node);
    }
  }

  static void
  OnUpdate(entt::registry& world, entt::entity node) {
    if (!world.any_of<UpdateLightTag>(node)) {
      world.emplace<UpdateLightTag>(node);
    }
  }

  static void
  OnDestroy(entt::registry& world, entt::entity node) {}
};

struct PointLightComponent {
  PointLight m_light;
};

struct SunLightTag {};

}  // namespace Marbas
