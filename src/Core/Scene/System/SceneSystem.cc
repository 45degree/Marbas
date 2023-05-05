#include "SceneSystem.hpp"

#include <glog/logging.h>

#include "AssetManager/ModelAsset.hpp"
#include "Core/Scene/Component/Component.hpp"
#include "Core/Scene/Component/MeshComponent.hpp"
#include "Core/Scene/GPUDataPipeline/LightGPUData.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

void
SceneSystem::UpdateTransformCompForEntity(Scene* scene, entt::entity entity) {
  auto& world = scene->GetWorld();
  if (!world.any_of<HierarchyComponent>(entity)) {
    return;
  }
  auto& hierarchyComponent = world.get<HierarchyComponent>(entity);
  if (world.any_of<TransformComp>(entity)) {
    auto& transformComp = world.get<TransformComp>(entity);
    auto parent = hierarchyComponent.parent;
    if (parent != entt::null) {
      auto& parentTransformComp = world.get<TransformComp>(parent);
      // if the transform Component has a update tranform matrix, it meanus it has beed set a global transform
      // so it no need to update it
      if (!transformComp.m_updatedGlobalTransform.has_value()) {
        auto& updateTransform = *transformComp.m_updatedGlobalTransform;
        if (parentTransformComp.m_updatedGlobalTransform.has_value()) {
          const auto& parentTransformMatrix = parentTransformComp.m_globalTransform;
          const auto& parentUpdateTransform = *parentTransformComp.m_updatedGlobalTransform;
          auto localTransformMatrix = glm::inverse(parentTransformMatrix) * transformComp.m_globalTransform;
          updateTransform = parentUpdateTransform * localTransformMatrix;
        }
      }
    }
  }

  for (auto& child : hierarchyComponent.children) {
    UpdateTransformCompForEntity(scene, child);
  }
}

void
SceneSystem::UpdateTransformComp(Scene* scene) {
  auto& world = scene->GetWorld();
  auto root = scene->GetRootNode();
  UpdateTransformCompForEntity(scene, root);

  auto view = world.view<TransformComp>();
  for (auto&& [entity, component] : view.each()) {
    if (component.m_updatedGlobalTransform.has_value()) {
      component.m_globalTransform = *component.m_updatedGlobalTransform;
      component.m_updatedGlobalTransform = std::nullopt;
    }
  }
}

void
SceneSystem::ClearUnuseAsset(Scene* scene) {
  AssetManager<TextureAsset>::GetInstance()->Tick();
  AssetManager<ModelAsset>::GetInstance()->Tick();
}

void
SceneSystem::UpdateAABBComponent(Scene* scene) {
  auto& world = scene->GetWorld();
  auto aabbView = world.view<ModelSceneNode>();
  for (auto&& [entity, modelSceneNode] : aabbView.each()) {
    auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();
    if (world.any_of<AABBComponent>(entity)) continue;
    if (modelSceneNode.modelPath == "res://") continue;

    auto model = modelAssetMgr->Get(modelSceneNode.modelPath);
    auto& aabb = world.emplace<AABBComponent>(entity, *model);
    LOG(INFO) << FORMAT("Add a aabb for model: {}, center: {}, extend: {}", modelSceneNode.modelPath,
                        glm::to_string(aabb.GetCenter()), glm::to_string(aabb.GetExtent()));
  }
}

Task<void>
SceneSystem::CreateAssetCache(Scene* scene) {
  auto& world = scene->GetWorld();
  auto modelNodeView = world.view<ModelSceneNode>();
  auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();
  for (auto&& [entity, node] : modelNodeView.each()) {
    if (node.modelPath == "res://") continue;
    if (!modelAssetMgr->Existed(node.modelPath)) {
      auto asset = co_await modelAssetMgr->CreateAsync(node.modelPath);
      LOG(INFO) << FORMAT("Create a model asset, path: {}, uid: {}", node.modelPath, asset->GetUid());
    }
  }
  co_return;
}

Task<void>
SceneSystem::LoadMesh(Scene* scene) {
  auto& world = scene->GetWorld();
  auto modelNodeView = world.view<ModelSceneNode, NewModelTag>();
  auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();

  for (auto&& [entity, node] : modelNodeView.each()) {
    if (node.modelPath == "res://") continue;
    auto asset = co_await modelAssetMgr->GetAsync(node.modelPath);
    node.m_meshEntities.clear();

    auto meshCount = asset->GetMeshCount();
    for (int i = 0; i < meshCount; i++) {
      auto meshEntity = world.create();
      auto& component = world.emplace<MeshComponent>(meshEntity);
      component.index = i;
      component.m_modelAsset = asset;
      node.m_meshEntities.push_back(meshEntity);
    }

    world.remove<NewModelTag>(entity);
  }

  co_return;
}

}  // namespace Marbas
