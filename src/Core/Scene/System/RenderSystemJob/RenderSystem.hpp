#pragma once

#include <entt/entt.hpp>

#include "Core/Renderer/RenderGraph/RenderGraphResourceManager.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHIFactory.hpp"
#include "RenderGraphJob.hpp"
#include "RenderLightDataJob.hpp"
#include "RenderMeshDataJob.hpp"
#include "RenderVXGIJob.hpp"
#include "RenderViewClipJob.hpp"

namespace Marbas::Job {

struct RenderInfo {
  Scene* scene;
  int imageIndex;
  Semaphore* waitSemaphore = nullptr;
  Semaphore* signalSemaphore = nullptr;
  Fence* fence = nullptr;
};

class RenderSystem {
 public:
  RenderSystem(RHIFactory* rhiFactory, std::shared_ptr<RenderGraph> renderGraph,
               std::shared_ptr<RenderGraph> precomputeGraph, std::shared_ptr<RenderGraphResourceManager>& res)
      : m_rhiFactory(rhiFactory),
        m_renderGraph(renderGraph),
        m_precomputeGraph(precomputeGraph),
        m_renderGraphResMgr(res){};

 public:
  void
  Init() {
    m_scheduler.attach<RenderGraphJob>(m_rhiFactory, m_renderGraph, m_precomputeGraph, m_renderGraphResMgr);
    m_scheduler.attach<RenderLightDataJob>(m_rhiFactory);
    m_scheduler.attach<RenderMeshDataJob>(m_rhiFactory);
    m_scheduler.attach<RenderViewClipJob>();
    m_scheduler.attach<RenderVXGIJob>(m_rhiFactory);
  }

  void
  Update(uint32_t deltaTime, RenderInfo& renderinfo) {
    m_scheduler.update(deltaTime, &renderinfo);
  }

 private:
  entt::scheduler m_scheduler;
  RHIFactory* m_rhiFactory;
  std::shared_ptr<RenderGraph> m_renderGraph;
  std::shared_ptr<RenderGraph> m_precomputeGraph;
  std::shared_ptr<RenderGraphResourceManager> m_renderGraphResMgr;
};

}  // namespace Marbas::Job
