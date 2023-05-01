#include "Core/Scene/Scene.hpp"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glog/logging.h>

#include <algorithm>
#include <assimp/Importer.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/cereal.hpp>
#include <fstream>
#include <strstream>

#include "Common/Common.hpp"
#include "Common/EditorCamera.hpp"
#include "Component/Component.hpp"
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
  world.on_construct<EmptySceneNode>().connect<&EmptySceneNode::RegistryNode>();
  RegistryNode<DirectionalLightSceneNode>(world);
  RegistryNode<DirectionShadowComponent>(world);
  RegistryNode<DirectionLightComponent>(world);
  world.on_construct<PointLightSceneNode>().connect<&PointLightSceneNode::RegistryNode>();
  world.on_construct<ModelSceneNode>().connect<&ModelSceneNode::RegistryNode>();
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

  entt::snapshot_loader loader{world};
  SerializeComponent(loader, archive);
  loader.orphans();

  auto scene = std::make_unique<Scene>(std::move(world));

  // FIX: 序列化之后, 没有触发on_construct方法, 导致有些Tag没有加上
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
  std::ofstream file(scenePath, std::ios::out | std::ios::binary);
  cereal::BinaryOutputArchive archive(file);

  entt::snapshot saver{m_world};
  SerializeComponent(saver, archive);
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
