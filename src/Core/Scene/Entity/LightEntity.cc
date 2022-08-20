#include "Core/Scene/Entity/LightEntity.hpp"

#include "Common/Light.hpp"
#include "Core/Scene/Component/LightComponent.hpp"

namespace Marbas {

LightEntity
LightPolicy::Create(entt::registry& registry) {
  auto light = registry.create();
  registry.emplace<LightComponent>(light);

  auto& lightComponent = registry.get<LightComponent>(light);
  lightComponent.m_light = std::make_unique<Light>();

  return light;
}

}  // namespace Marbas
