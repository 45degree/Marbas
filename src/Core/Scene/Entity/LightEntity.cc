#include "Core/Scene/Entity/LightEntity.hpp"

#include "Common/Light.hpp"
#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"

namespace Marbas {

LightEntity
LightPolicy::Create(entt::registry& registry, LightType lightType, const glm::vec3 pos) {
  auto light = registry.create();
  auto& tag = registry.emplace<UniqueTagComponent>(light);
  registry.emplace<HierarchyComponent>(light);
  tag.type = EntityType::Light;
  tag.tagName = "light";
  switch (lightType) {
    case LightType::ParalleLight:
      registry.emplace<ParallelLightComponent>(light).m_light.SetPos(pos);
      break;
    case LightType::PointLight:
      registry.emplace<PointLightComponent>(light).m_light.SetPos(pos);
      break;
  }

  return light;
}

}  // namespace Marbas
