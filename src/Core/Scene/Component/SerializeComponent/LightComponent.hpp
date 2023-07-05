#pragma once

#include <entt/entt.hpp>
#include <optional>

#include "../TagComponent.hpp"
#include "Common/MathCommon.hpp"

namespace Marbas {

struct DirectionLightComponent {
  std::optional<uint32_t> lightIndex;

  glm::vec3 m_color = glm::vec3(1, 1, 1);
  glm::vec3 m_direction = glm::vec3(0, 0, -1);
  float m_energy = 1;

  static void
  OnCreate(entt::registry& world, entt::entity node) {}

  static void
  OnUpdate(entt::registry& world, entt::entity node) {}

  static void
  OnDestroy(entt::registry& world, entt::entity node) {}

  template <typename Archive>
  void
  serialize(Archive&& archive) {
    archive(m_color, m_direction, m_energy);
  }
};

struct PointLightComponent {
  // details::PointLight m_light;
};

}  // namespace Marbas
