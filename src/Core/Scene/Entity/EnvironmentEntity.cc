#include "Core/Scene/Entity/EnvironmentEntity.hpp"

#include <entt/entt.hpp>

#include "Core/Scene/Component/EnvironmentComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"

namespace Marbas {

EnvironmentEntity
EnvironmentPolicy::Create(entt::registry& registry, const Path& hdrImagePath) {
  auto entity = registry.create();
  registry.emplace<EnvironmentComponent>(entity);
  registry.emplace<UniqueTagComponent>(entity);

  auto& component = registry.get<EnvironmentComponent>(entity);
  component.hdrImagePath = hdrImagePath;

  return entity;
}

}  // namespace Marbas
