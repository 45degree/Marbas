#include "SceneSystem.hpp"

#include <glog/logging.h>

#include "AssetManager/ModelAsset.hpp"
#include "Core/Scene/Component/Component.hpp"
#include "Core/Scene/Component/MeshComponent.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

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

Job::SceneSystem SceneSystem::s_sceneSystem;

void
SceneSystem::Initialize() {
  s_sceneSystem.Init();
}

Task<void>
SceneSystem::Update(Scene* scene) {
  // TODO: remove to the editor
  co_await CreateAssetCache(scene);

  static Scene* s_lastScene = nullptr;
  Job::SceneUserData userData;
  userData.m_scene = scene;
  if (scene != s_lastScene) {
    userData.m_sceneChange = true;
    s_lastScene = scene;
  } else {
    userData.m_sceneChange = false;
  }

  // update scene aabb
  s_sceneSystem.Update(0, &userData);

  co_return;
}

}  // namespace Marbas
