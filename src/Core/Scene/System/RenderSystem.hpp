#pragma once

#include <entt/entt.hpp>

#include "Core/Renderer/RenderGraph/RenderGraphResourceManager.hpp"
#include "Core/Scene/Scene.hpp"
#include "Core/Scene/System/RenderSystemJob/RenderSystem.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

struct RenderInfo final {
  Scene* scene;
  int imageIndex;
  Semaphore* waitSemaphore = nullptr;
  Semaphore* signalSemaphore = nullptr;
  Fence* fence = nullptr;
};

struct RenderSystem final {
  static void
  Initialize(RHIFactory* rhiFactory);

  static void
  Update(const RenderInfo& renderInfo);

  static void
  Destroy(RHIFactory* rhiFactory);

  static void
  RerunPreComputePass(const StringView& passName, RHIFactory* rhiFactory);

  static RenderGraphResourceManager*
  GetRenderGraphResourceManager() {
    return s_resourceManager.get();
  }

 private:
  static std::shared_ptr<Job::RenderSystem> s_renderSystem;
  static std::shared_ptr<RenderGraphResourceManager> s_resourceManager;
  static std::shared_ptr<RenderGraph> s_renderGraph;
  static std::shared_ptr<RenderGraph> s_precomputeRenderGraph;
};

}  // namespace Marbas
