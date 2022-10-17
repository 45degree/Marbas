#include "Core/Scene/System/HierarchySystem.hpp"

#include "Core/Scene/Component/HierarchyComponent.hpp"

namespace Marbas {

void
HierarchySystem::UpdateTransformMatrix(entt::registry& registry) {
  auto view = registry.view<HierarchyComponent>();
  for (auto&& [entity, component] : view.each()) {
    if (component.parent == entt::null) {
      component.globalTransformMatrix = component.localTransformMatrix;
    } else {
      const auto& parentComponent = registry.get<HierarchyComponent>(component.parent);
      component.globalTransformMatrix =
          parentComponent.globalTransformMatrix * component.globalTransformMatrix;
    }
  }
}

void
HierarchySystem::UpdateChildrenTransformMatrix(entt::registry& registry, entt::entity parent) {
  const auto& currentComponent = registry.get<HierarchyComponent>(parent);
  const auto& currentGlobalMatrix = currentComponent.globalTransformMatrix;
  for (auto child : currentComponent.children) {
    auto& childComponent = registry.get<HierarchyComponent>(child);
    childComponent.globalTransformMatrix =
        currentGlobalMatrix * childComponent.localTransformMatrix;
    UpdateChildrenTransformMatrix(registry, child);
  }
}

void
HierarchySystem::ResetGlobalTransformMatrix(entt::registry& registry, entt::entity current,
                                            const glm::mat4& globalTransformMatrix) {
  auto& currentComponent = registry.get<HierarchyComponent>(current);
  currentComponent.globalTransformMatrix = globalTransformMatrix;
  auto parent = currentComponent.parent;

  const auto& parentComponent = registry.get<HierarchyComponent>(parent);
  auto local = glm::inverse(parentComponent.globalTransformMatrix) * globalTransformMatrix;
  currentComponent.localTransformMatrix = local;

  UpdateChildrenTransformMatrix(registry, current);
}

}  // namespace Marbas
