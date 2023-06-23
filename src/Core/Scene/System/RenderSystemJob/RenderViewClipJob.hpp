#pragma once

#include <entt/entt.hpp>

namespace Marbas::Job {

class RenderViewClipJob : public entt::process<RenderViewClipJob, uint32_t> {
  using DeltaTime = uint32_t;

 public:
  void
  update(DeltaTime deltaTime, void* data);
};

}  // namespace Marbas::Job
