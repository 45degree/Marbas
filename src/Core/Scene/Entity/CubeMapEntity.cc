#include "Core/Scene/Entity/CubeMapEntity.hpp"

#include <entt/entt.hpp>

#include "Core/Scene/Component/CubeMapComponent.hpp"
#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"

namespace Marbas {

CubeMapEntity
CubeMapPolicy::Create(entt::registry& registry, Uid cubeMapResourceId) {
  auto cubeMap = registry.create();
  registry.emplace<CubeMapComponent>(cubeMap);
  registry.emplace<UniqueTagComponent>(cubeMap);
  registry.emplace<HierarchyComponent>(cubeMap);

  auto& tag = registry.get<UniqueTagComponent>(cubeMap);
  tag.type = EntityType::CubeMap;
  tag.tagName = "skybox";

  auto& cubeMapComponent = registry.get<CubeMapComponent>(cubeMap);
  cubeMapComponent.cubeMapResourceId = cubeMapResourceId;

  return cubeMap;
}

}  // namespace Marbas
