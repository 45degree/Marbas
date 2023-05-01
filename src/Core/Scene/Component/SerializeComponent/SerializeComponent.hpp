#pragma once

#include <entt/entity/registry.hpp>
#include <entt/entity/snapshot.hpp>

#include "HierarchyComponent.hpp"
#include "LightComponent.hpp"
#include "SceneNodeComponent.hpp"
#include "ShadowComponent.hpp"
#include "TransformComp.hpp"

namespace Marbas {

template <typename Snapshot, typename Archive>
void
SerializeComponent(Snapshot& snapshot, Archive& archive) {
  snapshot.entities(archive)
      .template component<EmptySceneNode>(archive)
      .template component<DirectionalLightSceneNode>(archive)
      .template component<PointLightSceneNode>(archive)
      .template component<ModelSceneNode>(archive)
      .template component<HierarchyComponent>(archive)
      .template component<TransformComp>(archive)
      .template component<DirectionLightComponent>(archive)
      .template component<DirectionShadowComponent>(archive);
}

}  // namespace Marbas
