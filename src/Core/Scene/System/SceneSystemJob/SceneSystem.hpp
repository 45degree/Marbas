#pragma once

#include "AABBJob.hpp"
#include "Core/Scene/Scene.hpp"
#include "LoadMeshJob.hpp"
#include "TransformJob.hpp"

namespace Marbas::Job {

struct SceneUserData {
  Scene* m_scene = nullptr;
  bool m_sceneChange = false;
};

class SceneSystem final {
 public:
  void
  Init() {
    m_scheduler.attach<TransformJob>();
    m_scheduler.attach<AABBJob>();
    m_scheduler.attach<LoadMeshJob>();
  }

  void
  Update(uint32_t deltaTime, SceneUserData* userData) {
    m_scheduler.update(deltaTime, userData);
  }

 private:
  entt::scheduler m_scheduler;
};

};  // namespace Marbas::Job
