#include "TransformJob.hpp"

#include "Core/Scene/Component/SerializeComponent/TransformComp.hpp"
#include "Core/Scene/Scene.hpp"
#include "Core/Scene/System/SceneSystemJob/SceneSystem.hpp"

namespace Marbas::Job {

void
TransformJob::update(uint32_t deltaTime, void* data) {
  auto* sceneUserData = reinterpret_cast<SceneUserData*>(data);
  if (sceneUserData->m_sceneChange && sceneUserData->m_scene != nullptr) {
    auto collector = entt::collector.update<TransformComp>();
    sceneUserData->m_scene->ConnectObserve(m_observer, collector);
    return;
  }

  if (!sceneUserData->m_sceneChange && m_observer.empty()) return;

  auto rootEntity = sceneUserData->m_scene->GetRootNode();
  UpdateTransformRecursion(sceneUserData->m_scene, rootEntity);

  m_observer.clear();
}

void
TransformJob::UpdateTransformRecursion(Scene* scene, entt::entity entity) {
  if (!scene->AnyOf<HierarchyComponent>(entity)) return;

  const auto& hierarchy = scene->Get<HierarchyComponent>(entity);
  if (hierarchy.parent == entt::null) {
    scene->Update<TransformComp>(entity, [&](auto& component) {
      if (!component.IsDirty()) return false;

      auto& localTrans = component.GetLocalTransform();
      component.SetLocalTransform(localTrans, glm::mat4(1.0));
      return false;  // not emit an update signal
    });
  } else {
    auto& parentEntity = hierarchy.parent;
    auto& parentTrans = scene->Get<TransformComp>(parentEntity);
    auto& parentGlobalTrans = parentTrans.GetGlobalTransform();
    scene->Update<TransformComp>(entity, [&](auto& component) {
      // if (!component.IsDirty()) return false;  // this componet has set global transform
      auto& localTrans = component.GetLocalTransform();
      component.SetLocalTransform(localTrans, parentGlobalTrans);
      return false;  // not emit an update signal
    });
  }

  for (auto child : hierarchy.children) {
    UpdateTransformRecursion(scene, child);
  }
}

};  // namespace Marbas::Job
