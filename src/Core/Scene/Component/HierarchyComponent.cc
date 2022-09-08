#include "Core/Scene/Component/HierarchyComponent.hpp"

#include "Core/Scene/Entity/Entity.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

void
HierarchyComponent::AddChild(const entt::entity parent, const std::shared_ptr<Scene>& scene,
                             entt::entity child) {
  auto& registry = const_cast<entt::registry&>(scene->GetWorld());
  AddChild(parent, registry, child);
}

void
HierarchyComponent::AddChild(const entt::entity parent, entt::registry& registry,
                             entt::entity child) {
  auto& parentComponent = registry.get<HierarchyComponent>(parent);
  auto& childComponent = registry.get<HierarchyComponent>(child);
  childComponent.parent = parent;

  if (!parentComponent.children.empty()) {
    auto lastChildEntity = parentComponent.children[parentComponent.children.size() - 1];
    auto& lastChildComponent = registry.get<HierarchyComponent>(lastChildEntity);
    lastChildComponent.next = child;
    childComponent.prew = lastChildEntity;
  }

  parentComponent.children.push_back(child);
}

}  // namespace Marbas
