#include "SceneSystem.hpp"

#include <glog/logging.h>

#include "AssetManager/ModelAsset.hpp"
#include "Core/Scene/Component/AABBComponent.hpp"
#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Component/RenderMeshComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"
#include "Core/Scene/Component/TransformComp.hpp"
#include "Core/Scene/GPUDataPipeline/LightGPUData.hpp"
#include "Core/Scene/GPUDataPipeline/ModelGPUData.hpp"
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
SceneSystem::UploadGPUAsset(Scene* scene) {
  AssetManager<TextureAsset>::GetInstance()->Tick();
  AssetManager<ModelAsset>::GetInstance()->Tick();
}

void
SceneSystem::UpdateAABBComponent(Scene* scene) {
  auto& world = scene->GetWorld();
  auto aabbView = world.view<ModelSceneNode>();
  for (auto&& [entity, modelSceneNode] : aabbView.each()) {
    auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();

    // TODO: change AABBComponent if model changed
    if (world.any_of<AABBComponent>(entity)) continue;
    if (modelSceneNode.modelPath == "res://") continue;

    auto model = modelAssetMgr->Get(modelSceneNode.modelPath);
    auto& aabb = world.emplace<AABBComponent>(entity, *model);
    LOG(INFO) << FORMAT("Add a aabb for model: {}, center: {}, extend: {}", modelSceneNode.modelPath,
                        glm::to_string(aabb.GetCenter()), glm::to_string(aabb.GetExtent()));
  }
}

void
SceneSystem::CreateAsset(Scene* scene) {
  auto& world = scene->GetWorld();
  auto modelNodeView = world.view<ModelSceneNode>();
  auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();
  for (auto [entity, node] : modelNodeView.each()) {
    if (node.modelPath == "res://") continue;
    if (!modelAssetMgr->Existed(node.modelPath)) {
      auto asset = modelAssetMgr->Create(node.modelPath);
      LOG(INFO) << FORMAT("Create a model asset, path: {}, uid: {}", node.modelPath, asset->GetUid());
    }
  }
}

void
SceneSystem::UpdateDirectionShadowInfo(Scene* scene) {
  auto& world = scene->GetWorld();
  auto camera = scene->GetEditorCamrea();
  auto view = world.view<DirectionShadowComponent, DirectionLightComponent>();
  for (auto&& [entity, shadow, light] : view.each()) {
    auto dir = light.m_light.GetDirection();
    world.patch<DirectionShadowComponent>(entity, [&dir, &camera](auto& node) {
      node.UpdateShadowInfo(dir, *camera);
      return;
    });
  }
}

void
SceneSystem::UpdateMeshGPUAsset(Scene* scene, RHIFactory* rhiFactory) {
  auto& world = scene->GetWorld();
  auto modelNodeView = world.view<ModelSceneNode>();
  auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();
  auto modelGPUAssetMgr = ModelGPUDataManager::GetInstance();

  for (auto [entity, node] : modelNodeView.each()) {
    if (node.modelPath == "res://") continue;
    if (!modelAssetMgr->Existed(node.modelPath)) continue;

    /**
     * sync the gpu data
     */
    auto modelAsset = modelAssetMgr->Get(node.modelPath);
    if (!modelGPUAssetMgr->Existed(*modelAsset)) continue;
    modelGPUAssetMgr->Update(*modelAsset);

    /**
     * clear flags
     */
    int meshCount = modelAsset->GetMeshCount();
    for (int i = 0; i < meshCount; i++) {
      modelAsset->GetMesh(i).m_materialTexChanged = false;
      modelAsset->GetMesh(i).m_materialValueChanged = false;
    }
  }
}

void
SceneSystem::UpdateLightGPUData(Scene* scene) {
  auto& world = scene->GetWorld();
  auto lightGPUDataMgr = LightGPUDataManager::GetInstance();

  /**
   * add all new ligth
   */
  auto newView = world.view<NewLightTag>();
  for (auto entity : newView) {
    // handle all directional light
    if (world.any_of<DirectionLightComponent>(entity)) {
      auto& light = world.get<DirectionLightComponent>(entity);
      if (world.any_of<DirectionShadowComponent>(entity)) {
        auto& shadow = world.get<DirectionShadowComponent>(entity);
        lightGPUDataMgr->Create(entity, light, shadow);
      } else {
        lightGPUDataMgr->Create(entity, light);
      }
    }

    // TODO: handle point light and spot light
  }

  for (auto entity : newView) {
    world.remove<NewLightTag>(entity);
  }

  /**
   * update light
   */
  auto updateView = world.view<UpdateLightTag>();
  for (auto entity : updateView) {
    if (world.any_of<DirectionShadowComponent>(entity)) {
      const auto& light = world.get<DirectionLightComponent>(entity);
      if (world.any_of<DirectionShadowComponent>(entity)) {
        const auto& shadow = world.get<DirectionShadowComponent>(entity);
        lightGPUDataMgr->Update(entity, light, shadow);
      } else {
        lightGPUDataMgr->Update(entity, light);
      }
    }

    // TODO: handle point light and spot light
  }
  for (auto entity : updateView) {
    world.remove<UpdateLightTag>(entity);
  }

  /**
   * remove light
   */
  auto deleteView = world.view<DeleteLightTag>();
  for (auto entity : deleteView) {
    if (world.any_of<DirectionShadowComponent>(entity)) {
      auto& light = world.get<DirectionShadowComponent>(entity);
      if (world.any_of<DirectionShadowComponent>(entity)) {
        auto& shadow = world.get<DirectionShadowComponent>(entity);
        // TODO: delete entity gpu data with light and shadow and delete the two component
      } else {
        // TODO: delete entity gpu data with light and delete the two component
      }
    }

    // TODO: handle point light and spot light
  }
  for (auto entity : deleteView) {
    world.remove<DeleteLightTag>(entity);
  }
}

}  // namespace Marbas
