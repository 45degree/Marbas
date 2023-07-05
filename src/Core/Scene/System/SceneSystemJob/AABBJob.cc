#include "AABBJob.hpp"

#include "Core/Scene/System/SceneSystemJob/SceneSystem.hpp"

namespace Marbas::Job {

template <typename Iterable>
void
UpdateAABB(Scene* scene, Iterable&& collection) {
  auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();
  for (auto entity : collection) {
    if (scene->AnyOf<AABBComponent>(entity)) continue;
    auto& modelSceneNode = scene->Get<ModelSceneNode>(entity);
    if (modelSceneNode.modelPath == "res://") continue;

    auto& modelPath = modelSceneNode.modelPath;
    auto model = modelAssetMgr->Get(modelPath);
    scene->Emplace<AABBComponent>(entity, *model);

    auto& aabb = scene->Get<AABBComponent>(entity);
    auto center = glm::to_string(aabb.GetCenter());
    auto extent = glm::to_string(aabb.GetExtent());
    LOG(INFO) << FORMAT("Add a aabb for model: {}, center: {}, extend: {}", modelPath, center, extent);
  }
}

void
AABBJob::update(uint32_t deltaTime, void* data) {
  auto* sceneUserData = reinterpret_cast<SceneUserData*>(data);

  if (sceneUserData->m_sceneChange && sceneUserData->m_scene != nullptr) {
    constexpr auto collector = entt::collector.group<ModelSceneNode>().update<ModelSceneNode>();
    sceneUserData->m_scene->ConnectObserve(m_observer, collector);
  }

  if (sceneUserData->m_sceneChange) {
    auto view = sceneUserData->m_scene->View<ModelSceneNode>();
    UpdateAABB(sceneUserData->m_scene, view);
  } else {
    UpdateAABB(sceneUserData->m_scene, m_observer);
  }

  m_observer.clear();
}

}  // namespace Marbas::Job
