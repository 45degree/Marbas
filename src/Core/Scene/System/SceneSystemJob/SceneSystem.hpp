#pragma once

#include "AABBJob.hpp"
#include "Core/Scene/Scene.hpp"
#include "TransformJob.hpp"

namespace Marbas::Job {

class SceneSystem final {
 public:
  void
  Init() {
    m_scheduler.attach<TransformJob>();
    m_scheduler.attach<AABBJob>();
  }

  void
  Update(uint32_t deltaTime, Scene* scene) {
    m_scheduler.update(deltaTime, scene);
  }

 private:
  entt::scheduler m_scheduler;
};

};  // namespace Marbas::Job
