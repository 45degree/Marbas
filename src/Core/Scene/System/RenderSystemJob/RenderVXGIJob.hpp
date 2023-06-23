#pragma once

#include <entt/entt.hpp>

#include "Core/Scene/Component/RenderComponent/VXGIRenderComponent.hpp"
#include "RHIFactory.hpp"

namespace Marbas::Job {

class RenderVXGIJob : public entt::process<RenderVXGIJob, uint32_t> {
  using DeltaTime = uint32_t;

 public:
  RenderVXGIJob(RHIFactory* rhiFactory);
  ~RenderVXGIJob() override;

 public:
  void
  update(DeltaTime deltaTime, void* data);

 private:
  RHIFactory* m_rhiFactory;

  std::array<entt::entity, VXGIGlobalComponent::maxProbeCount> m_activeProbe;
};

}  // namespace Marbas::Job
