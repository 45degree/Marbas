#include "AABBJob.hpp"

namespace Marbas::Job {

void
AABBJob::update(uint32_t deltaTime, void* data) {
  auto* scene = reinterpret_cast<Scene*>(data);
  auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();

  if (m_currentScene != scene && scene != nullptr) {
    constexpr auto collector = entt::collector.group<ModelSceneNode>().update<ModelSceneNode>();
    scene->ConnectObserve(m_observer, collector);
    m_currentScene = scene;
  }

  for (auto entity : m_observer) {
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

  m_observer.clear();
}

}  // namespace Marbas::Job
