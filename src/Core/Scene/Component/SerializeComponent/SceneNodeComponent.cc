#include "SceneNodeComponent.hpp"

#include "../TagComponent.hpp"
#include "Core/Scene/Component/AABBComponent.hpp"
#include "LightComponent.hpp"
#include "ShadowComponent.hpp"
#include "TagComponent.hpp"
#include "TransformComp.hpp"

namespace Marbas {

void
EmptySceneNode::OnCreate(entt::registry& world, entt::entity node) {
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
}

void
DirectionalLightSceneNode::OnUpdate(entt::registry& world, entt::entity node) {}

void
DirectionalLightSceneNode::OnDestroy(entt::registry& world, entt::entity node) {
  // TODO: implement
}

void
PointLightSceneNode::OnCreate(entt::registry& world, entt::entity node) {
  if (!world.any_of<TransformComp>(node)) {
    world.emplace<TransformComp>(node);
  }
}

void
ModelSceneNode::OnCreate(entt::registry& world, entt::entity node) {
  if (!world.any_of<TransformComp>(node)) {
    world.emplace<TransformComp>(node);
  }

  // if (!world.any_of<StaticModelTag>(node)) {
  //   world.emplace<StaticModelTag>(node);
  // }
}

void
VXGIProbeSceneNode::OnCreate(entt::registry& world, entt::entity node) {
  if (!world.any_of<TransformComp>(node)) {
    world.emplace<TransformComp>(node);
  }

  if (!world.any_of<NewVXGIProbeTag>(node)) {
    world.emplace<NewVXGIProbeTag>(node);
  }
}

}  // namespace Marbas
