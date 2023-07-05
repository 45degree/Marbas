#pragma once

#include <entt/entt.hpp>

namespace Marbas::Job {

class LoadMeshJob : public entt::process<LoadMeshJob, uint32_t> {
 public:
  void
  update(uint32_t deltaTime, void* data);

 private:
  entt::observer m_observer;
};

}  // namespace Marbas::Job
