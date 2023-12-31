#pragma once

#include <cereal/types/string.hpp>
#include <entt/entt.hpp>

#include "AssetManager/AssetPath.hpp"
#include "AssetManager/ModelAsset.hpp"
#include "Common/Common.hpp"
#include "Core/Scene/Component/TagComponent.hpp"

namespace Marbas {

// TODO: add some global entity to save all the directional light gpu data

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

  std::vector<entt::entity> m_meshEntities;

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
  AfterLoad(entt::registry& world, entt::entity node) {}

  static void
  AfterSave(entt::registry& world, entt::entity node) {
    auto modelManager = AssetManager<ModelAsset>::GetInstance();
    modelManager->Save();
  }
};

struct VXGIProbeSceneNode {
  std::string nodeName;
  glm::vec3 size = glm::vec3(4000, 4000, 4000);

  template <typename Archive>
  void
  serialize(Archive&& ar) {
    ar(nodeName, size);
  }

  static void
  OnCreate(entt::registry& world, entt::entity node);

  static void
  OnUpdate(entt::registry& world, entt::entity node) {}

  static void
  OnDestroy(entt::registry& world, entt::entity node) {}
};

}  // namespace Marbas
