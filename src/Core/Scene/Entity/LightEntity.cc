#include "Core/Scene/Entity/LightEntity.hpp"

#include "Common/Light.hpp"
#include "Core/Scene/Component/LightComponent.hpp"

namespace Marbas {

LightEntity
LightPolicy::Create(entt::registry& registry, LightType lightType) {
  auto light = registry.create();
  switch (lightType) {
    case LightType::ParalleLight:
      registry.emplace<ParallelLightComponent>(light);
      break;
  }

  return light;
}

}  // namespace Marbas
