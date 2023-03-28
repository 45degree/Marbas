#pragma once

#include <entt/entt.hpp>

#include "AssetManager/AssetPath.hpp"
#include "Common/Common.hpp"

namespace Marbas {

struct EmptySceneNode {
  String nodeName = "empty Node";

  template <typename Archive>
  void
  serialize(Archive&& ar) {
    ar(nodeName);
  }

  static void
  RegistryNode(entt::registry& world, entt::entity node);
};

struct DirectionalLightSceneNode {
  String nodeName = "direction light";

  template <typename Archive>
  void
  serialize(Archive&& ar) {
    ar(nodeName);
  }

  static void
  RegistryNode(entt::registry& world, entt::entity node);
};

struct PointLightSceneNode {
  String nodeName = "point light";

  template <typename Archive>
  void
  serialize(Archive&& ar) {
    ar(nodeName);
  }

  static void
  RegistryNode(entt::registry& world, entt::entity node);
};

struct ModelSceneNode {
  String modelName;
  AssetPath modelPath;

  template <typename Archive>
  void
  serialize(Archive&& ar) {
    ar(modelName, modelPath);
  }

  static void
  RegistryNode(entt::registry& world, entt::entity node);
};

}  // namespace Marbas
