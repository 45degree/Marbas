#include "SceneNodeComponent.hpp"

#include "../TagComponent.hpp"
#include "Core/Scene/Component/AABBComponent.hpp"
#include "LightComponent.hpp"
#include "ShadowComponent.hpp"
#include "TransformComp.hpp"

namespace Marbas {

void
EmptySceneNode::RegistryNode(entt::registry& world, entt::entity node) {
  world.emplace<TransformComp>(node);
}

void
DirectionalLightSceneNode::OnCreate(entt::registry& world, entt::entity node) {
  if (!world.any_of<TransformComp>(node)) {
    world.emplace<TransformComp>(node);
  }

  if (!world.any_of<DirectionLightComponent>(node)) {
    world.emplace<DirectionLightComponent>(node);
  }

  if (!world.any_of<DirectionShadowComponent>(node)) {
    world.emplace<DirectionShadowComponent>(node);
  }

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
