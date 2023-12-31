#pragma once

#include <entt/entt.hpp>

#include "Core/Scene/Scene.hpp"

namespace Marbas::Job {

class AABBJob final : public entt::process<AABBJob, uint32_t> {
 public:
  void
  update(uint32_t deltaTime, void* data);

 private:
  entt::observer m_observer;
};

}  // namespace Marbas::Job
