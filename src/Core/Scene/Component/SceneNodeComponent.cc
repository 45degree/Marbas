#include "SceneNodeComponent.hpp"

#include "Core/Scene/Component/AABBComponent.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Component/ShadowComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"
#include "Core/Scene/Component/TransformComp.hpp"

namespace Marbas {

void
EmptySceneNode::RegistryNode(entt::registry& world, entt::entity node) {
  world.emplace<TransformComp>(node);
}

void
DirectionalLightSceneNode::OnCreate(entt::registry& world, entt::entity node) {
  world.emplace<TransformComp>(node);
  world.emplace<DirectionLightComponent>(node);
  world.emplace<DirectionShadowComponent>(node);

  if (!world.any_of<NewLightTag>(node)) {
    world.emplace<NewLightTag>(node);
  }
}

void
DirectionalLightSceneNode::OnUpdate(entt::registry& world, entt::entity node) {
  if (!world.any_of<UpdateLightTag>(node)) {
    world.emplace<NewLightTag>(node);
  }
}

void
DirectionalLightSceneNode::OnDestroy(entt::registry& world, entt::entity node) {
  // TODO: implement
}

void
PointLightSceneNode::RegistryNode(entt::registry& world, entt::entity node) {
  world.emplace<TransformComp>(node);
}

void
ModelSceneNode::RegistryNode(entt::registry& world, entt::entity node) {
  world.emplace<TransformComp>(node);
}

}  // namespace Marbas
