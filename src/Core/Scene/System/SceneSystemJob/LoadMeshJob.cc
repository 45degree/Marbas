#include "LoadMeshJob.hpp"

#include "Core/Scene/System/SceneSystemJob/SceneSystem.hpp"

namespace Marbas::Job {

template <typename Iterable>
static void
CreateModelData(Scene* scene, Iterable&& iterable) {
  auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();
  for (auto& entity : iterable) {
    auto& node = scene->Get<ModelSceneNode>(entity);
    if (node.modelPath == "res://") continue;
    if (!node.m_meshEntities.empty()) continue;

    modelAssetMgr->GetAsync(node.modelPath).start([&](auto&& result) {
      if (result.hasError()) {
        LOG(INFO) << FORMAT("can't get asset:{}", node.modelPath);
        return;
      }

      auto asset = result.value();
      scene->Update<ModelSceneNode>(entity, [&](auto& node) {
        node.m_meshEntities.clear();

        auto meshCount = asset->GetMeshCount();
        for (int i = 0; i < meshCount; i++) {
          auto meshEntity = scene->CreateEntity();
          scene->Emplace<MeshComponent>(meshEntity);
          scene->Update<MeshComponent>(meshEntity, [&](auto& component) {
            component.index = i;
            component.m_modelAsset = asset;
            return true;
          });
          node.m_meshEntities.push_back(meshEntity);
        }
        return true;
      });
    });
  }
}

void
LoadMeshJob::update(uint32_t deltaTime, void* data) {
  auto* sceneUserData = reinterpret_cast<SceneUserData*>(data);
  auto* scene = sceneUserData->m_scene;
  if (sceneUserData->m_sceneChange) {
    auto modelNodeView = scene->View<ModelSceneNode>();
    CreateModelData(scene, modelNodeView);

    constexpr auto collector = entt::collector.group<ModelSceneNode>().update<ModelSceneNode>();
    scene->ConnectObserve(m_observer, collector);
  } else {
    CreateModelData(scene, m_observer);
  }
  m_observer.clear();

  AssetManager<TextureAsset>::GetInstance()->Tick();
  AssetManager<ModelAsset>::GetInstance()->Tick();
  return;
}

}  // namespace Marbas::Job
