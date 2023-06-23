#pragma once

#include <entt/entt.hpp>

#include "Core/Renderer/RenderGraph/RenderGraph.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphResourceManager.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHIFactory.hpp"

namespace Marbas::Job {

class RenderGraphJob final : public entt::process<RenderGraphJob, uint32_t> {
  using DeltaTime = uint32_t;

 public:
  RenderGraphJob(RHIFactory* rhiFactory, std::shared_ptr<RenderGraph>& graph,
                 std::shared_ptr<RenderGraph>& precomputeGraph, std::shared_ptr<RenderGraphResourceManager> resMgr);
  ~RenderGraphJob() override;

 public:
  void
  init();

  void
  update(DeltaTime deltaTime, void* data);

 private:
  void
  CreateRenderGraphResource();

  void
  CreateRenderGraphPass();

 private:
  RHIFactory* m_rhiFactory = nullptr;
  Fence* m_precomputeFence = nullptr;
  std::shared_ptr<RenderGraphResourceManager> m_resMgr = nullptr;
  std::shared_ptr<RenderGraph> m_renderGraph;
  std::shared_ptr<RenderGraph> m_precomputeRenderGraph;
};

}  // namespace Marbas::Job
