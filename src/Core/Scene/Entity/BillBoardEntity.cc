#include "Core/Scene/Entity/BillBoardEntity.hpp"

#include <entt/entt.hpp>

#include "Core/Scene/Component/BillBoardComponent.hpp"
#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"

namespace Marbas {

BillBoardEntity
BillBoardPolicy::Create(entt::registry& registry, Uid texture2DResourceId) {
  auto entity = registry.create();

  registry.emplace<BillBoardComponent>(entity);
  registry.emplace<UniqueTagComponent>(entity);
  registry.emplace<HierarchyComponent>(entity);

  auto& tag = registry.get<UniqueTagComponent>(entity);
  tag.type = EntityType::CubeMap;
  tag.tagName = "billBoard";

  auto& billBoardComponent = registry.get<BillBoardComponent>(entity);
  billBoardComponent.textureResourceId = texture2DResourceId;

  return entity;
}

}  // namespace Marbas
