#pragma once

#include <entt/entt.hpp>

#include "Core/Scene/Scene.hpp"
#include "RHIFactory.hpp"

namespace Marbas::Job {

class RenderLightDataJob final : public entt::process<RenderLightDataJob, uint32_t> {
  using DeltaTime = uint32_t;

 public:
  RenderLightDataJob(RHIFactory* rhiFactory) : m_rhiFactory(rhiFactory) {}
  ~RenderLightDataJob() override = default;

 public:
  void
  update(DeltaTime deltaTime, void* data);

 private:
  void
  UpdateDirectionShadowInfo(Scene* scene);

  void
  UpdateShadowMapAtlasPosition(Scene* scene);

 private:
  RHIFactory* m_rhiFactory;
};

}  // namespace Marbas::Job
