#include "SceneSystem.hpp"

#include <glog/logging.h>

#include "AssetManager/ModelAsset.hpp"
#include "Core/Scene/Component/Component.hpp"
#include "Core/Scene/Component/MeshComponent.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

void
SceneSystem::ClearUnuseAsset(Scene* scene) {
  AssetManager<TextureAsset>::GetInstance()->Tick();
  AssetManager<ModelAsset>::GetInstance()->Tick();
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
  auto modelNodeView = scene->View<ModelSceneNode, NewModelTag>();
  auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();

  for (auto& entity : modelNodeView) {
    auto& node = scene->Get<ModelSceneNode>(entity);
    if (node.modelPath == "res://") continue;

    auto asset = co_await modelAssetMgr->GetAsync(node.modelPath);
    scene->Update<ModelSceneNode>(entity, [&](auto& component) {
      component.m_meshEntities.clear();

      auto meshCount = asset->GetMeshCount();
      for (int i = 0; i < meshCount; i++) {
        auto meshEntity = scene->CreateEntity();
        scene->Emplace<MeshComponent>(meshEntity);
        scene->Update<MeshComponent>(meshEntity, [&](auto& component) {
          component.index = i;
          component.m_modelAsset = asset;
          return true;
        });
        component.m_meshEntities.push_back(meshEntity);
      }
      return true;
    });
    scene->Remove<NewModelTag>(entity);
  }
  co_return;
}

Job::SceneSystem SceneSystem::s_sceneSystem;

void
SceneSystem::Initialize() {
  s_sceneSystem.Init();
}

Task<void>
SceneSystem::Update(Scene* scene) {
  co_await CreateAssetCache(scene);
  co_await LoadMesh(scene);
  ClearUnuseAsset(scene);

  // update scene aabb
  s_sceneSystem.Update(0, scene);

  co_return;
}

}  // namespace Marbas
