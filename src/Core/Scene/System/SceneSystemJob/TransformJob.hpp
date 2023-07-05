#pragma once

#include <entt/entt.hpp>

#include "Core/Scene/Scene.hpp"

namespace Marbas::Job {

class TransformJob : public entt::process<TransformJob, uint32_t> {
 public:
  void
  update(uint32_t deltaTime, void* data);

 private:
  void
  UpdateTransformRecursion(Scene* scene, entt::entity entity);

 private:
  entt::observer m_observer;
  // Scene* m_currentObserverScene = nullptr;
};

}  // namespace Marbas::Job
