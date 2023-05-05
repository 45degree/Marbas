#include "Core/Scene/Scene.hpp"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glog/logging.h>

#include <algorithm>
#include <assimp/Importer.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>
#include <fstream>
#include <strstream>

#include "Common/Common.hpp"
#include "Common/EditorCamera.hpp"
#include "Component/Component.hpp"
#include "Core/Scene/Component/SerializeComponent/SerializeComponent.hpp"
#include "entt/entity/fwd.hpp"

namespace Marbas {

template <typename T>
static void
RegistryNode(entt::registry& world) {
  world.on_construct<T>().template connect<&T::OnCreate>();
  world.on_update<T>().template connect<&T::OnUpdate>();
  world.on_destroy<T>().template connect<&T::OnDestroy>();
}

static void
RegistryNodes(entt::registry& world) {
  RegistryNode<EmptySceneNode>(world);
  RegistryNode<DirectionalLightSceneNode>(world);
  RegistryNode<DirectionShadowComponent>(world);
  RegistryNode<DirectionLightComponent>(world);
  RegistryNode<PointLightSceneNode>(world);
  RegistryNode<ModelSceneNode>(world);
}

Scene::Scene() {
  RegistryNodes(m_world);

  m_rootEntity = AddChild(entt::null);
  auto& emptySceneNode = m_world.emplace<EmptySceneNode>(m_rootEntity);
  emptySceneNode.nodeName = "default scene";

  // TODO: remove
  auto& environmentComponent = m_world.emplace<EnvironmentComponent>(m_rootEntity);

  m_editorCamera = std::make_shared<EditorCamera>();
}

Scene::Scene(entt::registry&& registry) : m_world(std::move(registry)) {
  RegistryNodes(m_world);

  // find root entity
  auto view = m_world.view<HierarchyComponent>();
  for (auto [entity, hierarchyComponent] : view.each()) {
    if (hierarchyComponent.parent == entt::null) {
      m_rootEntity = entity;
      break;
    }
  }
  m_editorCamera = std::make_shared<EditorCamera>();

  if (m_rootEntity == entt::null) {
    LOG(ERROR) << "can't find root entity from world";
  }
}

std::unique_ptr<Scene>
Scene::LoadFromFile(const Path& scenePath) {
  std::ifstream file(scenePath, std::ios::in | std::ios::binary);
  cereal::BinaryInputArchive archive(file);

  entt::registry world;
  SerializeComponent(world, archive);

  auto scene = std::make_unique<Scene>(std::move(world));

  auto ExecuteCreateFunc = [&scene]<typename T>(T t) {
    auto& world = scene->GetWorld();
    auto view = world.view<T>();
    for (auto entity : view) {
      T::OnCreate(world, entity);
    }
    return;
  };
  ExecuteCreateFunc(DirectionalLightSceneNode());
  ExecuteCreateFunc(DirectionLightComponent());
  ExecuteCreateFunc(DirectionShadowComponent());

  return scene;
}

void
Scene::SaveToFile(const Path& scenePath) {
  if (std::filesystem::exists(scenePath)) {
    std::filesystem::remove(scenePath);
  }
  std::ofstream file(scenePath, std::ios::out | std::ios::binary);
  cereal::BinaryOutputArchive archive(file);
  SerializeComponent(m_world, archive);
}

entt::entity
Scene::AddChild(entt::entity parent) {
  auto entity = m_world.create();
  m_world.emplace<HierarchyComponent>(entity);
  HierarchyComponent::AddChild(parent, m_world, entity);
  return entity;
}

Vector<entt::entity>
Scene::GetChildren(entt::entity node) const {
  auto& hierarchyComponent = m_world.get<HierarchyComponent>(node);
  return hierarchyComponent.children;
}

entt::entity
Scene::GetChild(entt::entity node, size_t index) const {
  auto& hierarchyComponent = m_world.get<HierarchyComponent>(node);
  return hierarchyComponent.children[index];
}

size_t
Scene::GetChildrenCount(entt::entity node) const {
  auto& hierarchyComponent = m_world.get<HierarchyComponent>(node);
  return hierarchyComponent.children.size();
}

}  // namespace Marbas
