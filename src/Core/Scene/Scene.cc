#include "Core/Scene/Scene.hpp"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glog/logging.h>

#include <algorithm>
#include <assimp/Importer.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/cereal.hpp>
#include <fstream>
#include <strstream>

#include "Common/Common.hpp"
#include "Common/EditorCamera.hpp"
#include "Core/Scene/Component/EnvironmentComponent.hpp"
#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Component/SceneNodeComponent.hpp"
#include "Core/Scene/Component/TransformComp.hpp"
#include "entt/entity/fwd.hpp"

namespace Marbas {

Scene::Scene() {
  m_world.on_construct<EmptySceneNode>().connect<&EmptySceneNode::RegistryNode>();
  m_world.on_construct<DirectionalLightSceneNode>().connect<&DirectionalLightSceneNode::RegistryNode>();
  m_world.on_construct<PointLightSceneNode>().connect<&PointLightSceneNode::RegistryNode>();
  m_world.on_construct<ModelSceneNode>().connect<&ModelSceneNode::RegistryNode>();

  m_rootEntity = AddChild(entt::null);
  auto& emptySceneNode = m_world.emplace<EmptySceneNode>(m_rootEntity);
  emptySceneNode.nodeName = "default scene";

  // TODO: remove
  auto& environmentComponent = m_world.emplace<EnvironmentComponent>(m_rootEntity);

  m_editorCamera = std::make_shared<EditorCamera>();
}

Scene::Scene(entt::registry&& registry) : m_world(std::move(registry)) {
  // find root entity
  auto view = m_world.view<HierarchyComponent>();
  for (auto [entity, hierarchyComponent] : view.each()) {
    if (hierarchyComponent.parent == entt::null) {
      m_rootEntity = entity;
      break;
    }
  }
  m_editorCamera = std::make_shared<EditorCamera>();
}

std::shared_ptr<Scene>
Scene::LoadFromFile(const Path& scenePath) {
  std::ifstream file(scenePath, std::ios::in);
  cereal::XMLInputArchive archive(file);

  entt::registry world;

  entt::snapshot_loader{world}
      .entities(archive)
      .component<EmptySceneNode>(archive)
      .component<DirectionalLightSceneNode>(archive)
      .component<PointLightSceneNode>(archive)
      .component<ModelSceneNode>(archive)
      .component<HierarchyComponent>(archive)
      .orphans();

  auto scene = std::make_shared<Scene>(std::move(world));
  return scene;
}

void
Scene::SaveToFile(const Path& scenePath) {
  std::ofstream file(scenePath, std::ios::out);
  cereal::XMLOutputArchive archive(file);

  entt::snapshot{m_world}
      .entities(archive)
      .component<EmptySceneNode>(archive)
      .component<DirectionalLightSceneNode>(archive)
      .component<PointLightSceneNode>(archive)
      .component<ModelSceneNode>(archive)
      .component<HierarchyComponent>(archive);
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

// AABB
// Scene::GetSceneAABB() const {
//   auto view = m_world.view<ModelSceneNode, TransformComp>();
//   Vector<AABB> modelAABB;
//   // for (auto&& [entity, sceneNode, transform] : view.each()) {
//   //   AABB aabb(glm::vec3(0), glm::vec3(0));
//   //   auto newCenter = glm::vec3(transform.GetGlobalTransform() * glm::vec4(sceneNode.aabb.GetCenter(), 1));
//   //   aabb.SetCenter(newCenter);
//   //   aabb.SetExtent(sceneNode.aabb.GetExtent());
//   //   modelAABB.push_back(aabb);
//   // }
//
//   if (modelAABB.empty()) {
//     return AABB(glm::vec3(0), glm::vec3(0));
//   }
//   return AABB::CombineAABB(modelAABB);
// }

}  // namespace Marbas
