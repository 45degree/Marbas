#pragma once

#include <glog/logging.h>

#include <cereal/cereal.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/snapshot.hpp>

#include "Core/Scene/Component/TagComponent.hpp"
#include "EnvironmentComponent.hpp"
#include "HierarchyComponent.hpp"
#include "LightComponent.hpp"
#include "SceneNodeComponent.hpp"
#include "ShadowComponent.hpp"
#include "TagComponent.hpp"
#include "TransformComp.hpp"

namespace Marbas {

namespace details {

/**
 * execute after load
 */

template <typename... Components>
struct ExecuteAfterLoad;

template <>
struct ExecuteAfterLoad<> {
  void
  operator()(entt::registry& world, entt::entity node){};
};

template <typename Component, typename... Components>
struct ExecuteAfterLoad<Component, Components...> {
  define_has_member(AfterLoad);

  void
  operator()(entt::registry& world, entt::entity node) {
    if constexpr (has_member(Component, AfterLoad)) {
      if (world.any_of<Component>(node)) {
        Component::AfterLoad(world, node);
      }
    }
    ExecuteAfterLoad<Components...>()(world, node);
  }
};

/**
 * execute after save
 */

template <typename... Components>
struct ExecuteAfterSave;

template <>
struct ExecuteAfterSave<> {
  void
  operator()(entt::registry& world, entt::entity node) {}
};

template <typename Component, typename... Components>
struct ExecuteAfterSave<Component, Components...> {
  define_has_member(AfterSave);

  void
  operator()(entt::registry& world, entt::entity node) {
    if constexpr (has_member(Component, AfterSave)) {
      if (world.any_of<Component>(node)) {
        Component::AfterSave(world, node);
      }
    }
    ExecuteAfterSave<Components...>()(world, node);
  }
};

}  // namespace details

#define SerializeComponents                                                                                          \
  EmptySceneNode, DirectionalLightSceneNode, PointLightSceneNode, ModelSceneNode, HierarchyComponent, TransformComp, \
      DirectionLightComponent, DirectionShadowComponent, EnvironmentComponent, SunLightTag

template <typename ArchiveType, uint32_t flags = 0>
void
SerializeComponent(entt::registry& world, cereal::InputArchive<ArchiveType, flags>& archive) {
  entt::snapshot_loader loader{world};
  loader.entities(archive);
  loader.component<SerializeComponents>(archive);
  loader.orphans();

  auto view = world.view<entt::entity>();
  for (auto entity : view) {
    if (world.any_of<SerializeComponents>(entity)) {
      details::ExecuteAfterLoad<SerializeComponents>()(world, entity);
    }
  }
}

template <typename ArchiveType, uint32_t flags = 0>
void
SerializeComponent(entt::registry& world, cereal::OutputArchive<ArchiveType, flags>& archive) {
  std::vector<entt::entity> view;
  for (auto entity : world.view<entt::entity>()) {
    if (world.any_of<SerializeComponents>(entity)) {
      view.push_back(entity);
    }
  }

  entt::snapshot snapshot{world};
  snapshot.entities(archive);
  snapshot.component<SerializeComponents>(archive);

  for (auto entity : view) {
    if (world.any_of<SerializeComponents>(entity)) {
      details::ExecuteAfterSave<SerializeComponents>()(world, entity);
    }
  }
}

}  // namespace Marbas
