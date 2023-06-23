#pragma once

#include <entt/entt.hpp>

#include "Core/Scene/Scene.hpp"

namespace Marbas::Job {

class AABBJob : public entt::process<AABBJob, uint32_t> {
 public:
  void
  update(uint32_t deltaTime, void* data);

 private:
  entt::observer m_observer;
  Scene* m_currentScene = nullptr;
};

}  // namespace Marbas::Job
