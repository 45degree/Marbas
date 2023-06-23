#include "TransformJob.hpp"

#include "Core/Scene/Component/SerializeComponent/TransformComp.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas::Job {

void
TransformJob::update(uint32_t deltaTime, void* data) {
  auto* scene = reinterpret_cast<Scene*>(data);
  if (scene != m_currentObserverScene && scene != nullptr) {
    auto collector = entt::collector.update<TransformComp>();
    scene->ConnectObserve(m_observer, collector);
    m_currentObserverScene = scene;
    return;
  }

  if (m_observer.empty()) return;

  auto rootEntity = scene->GetRootNode();
  UpdateTransformRecursion(scene, rootEntity);

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
