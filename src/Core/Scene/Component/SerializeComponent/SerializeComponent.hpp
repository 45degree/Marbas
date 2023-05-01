#pragma once

#include <entt/entity/registry.hpp>
#include <entt/entity/snapshot.hpp>

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
  operator()(){};
};

template <typename Component, typename... Components>
struct ExecuteAfterLoad<Component, Components...> {
  define_has_member(AfterLoad);

  void
  operator()() {
    if constexpr (has_member(Component, AfterLoad)) {
      Component::AfterLoad();
    }
    ExecuteAfterLoad<Components...>()();
  }
};

/**
 * serialize component
 */

template <typename Snapshot, typename Archive, typename... Components>
struct SerializeComponent;

template <typename Snapshot, typename Archive>
struct SerializeComponent<Snapshot, Archive> {
  void
  operator()(Snapshot& snapshot, Archive& archive) {}
};

template <typename Snapshot, typename Archive, typename Component, typename... Components>
struct SerializeComponent<Snapshot, Archive, Component, Components...> {
  void
  operator()(Snapshot& snapshot, Archive& archive) {
    snapshot.template component<Component>(archive);
    SerializeComponent<Snapshot, Archive, Components...>()(snapshot, archive);
  }
};

}  // namespace details

#define SerializeComponents                                                                                          \
  EmptySceneNode, DirectionalLightSceneNode, PointLightSceneNode, ModelSceneNode, HierarchyComponent, TransformComp, \
      DirectionLightComponent, DirectionShadowComponent, EnvironmentComponent, SunLightTag

inline void
ExecuteAfterLoad() {
  details::ExecuteAfterLoad<SerializeComponents>()();
}

template <typename Snapshot, typename Archive>
void
SerializeComponent(Snapshot& snapshot, Archive& archive) {
  snapshot.entities(archive);
  details::SerializeComponent<Snapshot, Archive, SerializeComponents>()(snapshot, archive);
}

}  // namespace Marbas
