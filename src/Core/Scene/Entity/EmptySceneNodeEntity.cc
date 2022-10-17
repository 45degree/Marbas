#include "Core/Scene/Entity/EmptySceneNodeEntity.hpp"

#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"

namespace Marbas {

EmptySceneNodeEntity
EmptySceneNodeEntityPolicy::Create(entt::registry& registry) {
  auto emptySceneNodeEntity = registry.create();
  registry.emplace<HierarchyComponent>(emptySceneNodeEntity);
  auto& component = registry.emplace<UniqueTagComponent>(emptySceneNodeEntity);
  component.tagName = "empty node";
  component.type = EntityType::Emtpy;
  component.uid = Uid();

  return emptySceneNodeEntity;
}

}  // namespace Marbas
