#pragma once

#include <cereal/types/string.hpp>
#include <entt/entt.hpp>

#include "AssetManager/AssetPath.hpp"
#include "AssetManager/ModelAsset.hpp"
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
  OnCreate(entt::registry& world, entt::entity node);

  static void
  OnUpdate(entt::registry& world, entt::entity node) {}

  static void
  OnDestroy(entt::registry& world, entt::entity node) {}
};

struct DirectionalLightSceneNode {
  String nodeName = "direction light";

  template <typename Archive>
  void
  serialize(Archive&& ar) {
    ar(nodeName);
  }

  static void
  OnCreate(entt::registry& world, entt::entity node);

  static void
  OnUpdate(entt::registry& world, entt::entity node);

  static void
  OnDestroy(entt::registry& world, entt::entity node);
};

struct PointLightSceneNode {
  String nodeName = "point light";

  template <typename Archive>
  void
  serialize(Archive&& ar) {
    ar(nodeName);
  }

  static void
  OnCreate(entt::registry& world, entt::entity node);

  static void
  OnUpdate(entt::registry& world, entt::entity node) {}

  static void
  OnDestroy(entt::registry& world, entt::entity node) {}
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
  OnCreate(entt::registry& world, entt::entity node);

  static void
  OnUpdate(entt::registry& world, entt::entity node) {}

  static void
  OnDestroy(entt::registry& world, entt::entity node) {}

  static void
  AfterLoad() {
    auto modelManager = AssetManager<ModelAsset>::GetInstance();
    modelManager->Save();
  }
};

}  // namespace Marbas
