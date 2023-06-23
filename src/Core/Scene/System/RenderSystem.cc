#include "RenderSystem.hpp"

#include <glog/logging.h>

#include "AssetManager/ModelAsset.hpp"
#include "AssetManager/Singleton.hpp"
#include "Core/Renderer/GI/VXGI/LightInjectPass.hpp"
#include "Core/Renderer/GI/VXGI/VXGIPass.hpp"
#include "Core/Renderer/GI/VXGI/VoxelVisualizatonPass.hpp"
#include "Core/Renderer/GI/VXGI/VoxelizationPass.hpp"
#include "Core/Renderer/Pass/AtmospherePass.hpp"
#include "Core/Renderer/Pass/DirectLightPass.hpp"
#include "Core/Renderer/Pass/DirectionLightShadowMapPass.hpp"
#include "Core/Renderer/Pass/ForwardPass/GridPass.hpp"
#include "Core/Renderer/Pass/ForwardPass/SkyImagePass.hpp"
#include "Core/Renderer/Pass/GeometryPass.hpp"
#include "Core/Renderer/Pass/PreComputePass/MultiScatterLUT.hpp"
#include "Core/Renderer/Pass/PreComputePass/TransmittanceLUT.hpp"
#include "Core/Renderer/Pass/SSAOPass.hpp"
#include "Core/Renderer/RenderGraph/RenderGraph.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphResourceManager.hpp"
#include "Core/Scene/Component/Component.hpp"

namespace Marbas {

std::shared_ptr<RenderGraphResourceManager> RenderSystem::s_resourceManager;
std::shared_ptr<Job::RenderSystem> RenderSystem::s_renderSystem;
std::shared_ptr<RenderGraph> RenderSystem::s_renderGraph;
std::shared_ptr<RenderGraph> RenderSystem::s_precomputeRenderGraph;

static std::optional<ImageView*> s_lastResultImageView;

void
RenderSystem::Initialize(RHIFactory* rhiFactory) {
  uint32_t width = 800;
  uint32_t height = 600;

  /**
   * create all resource
   */
  s_resourceManager = std::make_shared<RenderGraphResourceManager>(rhiFactory);
  s_renderGraph = std::make_unique<RenderGraph>(rhiFactory, s_resourceManager);
  s_precomputeRenderGraph = std::make_unique<RenderGraph>(rhiFactory, s_resourceManager);
  s_renderSystem =
      std::make_shared<Job::RenderSystem>(rhiFactory, s_renderGraph, s_precomputeRenderGraph, s_resourceManager);
  s_renderSystem->Init();
}

void
RenderSystem::Destroy(RHIFactory* rhiFactory) {
  s_renderGraph = nullptr;
  s_precomputeRenderGraph = nullptr;
  s_resourceManager = nullptr;
  s_renderSystem = nullptr;
};

void
RenderSystem::Update(const RenderInfo& renderInfo) {
  Job::RenderInfo info;
  info.scene = renderInfo.scene;
  info.fence = renderInfo.fence;
  info.waitSemaphore = renderInfo.waitSemaphore;
  info.signalSemaphore = renderInfo.signalSemaphore;
  info.imageIndex = renderInfo.imageIndex;
  s_renderSystem->Update(0, info);
}

void
RenderSystem::RerunPreComputePass(const StringView& passName, RHIFactory* rhiFactory) {
  // /**
  //  * precompute all pass
  //  */
  // s_precomputeRenderGraph->ExecuteAlone(passName, nullptr, nullptr, nullptr, s_precomputeFence);
  // rhiFactory->WaitForFence(s_precomputeFence);
  // rhiFactory->ResetFence(s_precomputeFence);
  // LOG(INFO) << FORMAT("re-run the precompute pass:{}.", passName);
}

}  // namespace Marbas
