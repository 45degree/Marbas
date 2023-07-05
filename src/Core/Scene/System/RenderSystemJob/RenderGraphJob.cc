#include "RenderGraphJob.hpp"

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
#include "Core/Scene/System/RenderSystemJob/RenderSystem.hpp"

#define GBUFFER_NORMAL "normalTexture"
#define GBUFFER_POSITION "positionTexture"
#define GBUFFER_DEPTH "depthTexture"
#define GBUFFER_DIFFUSE "colorTexture"
#define GBUFFER_SSAO "ssaoTexture"
#define GBUFFER_DIRECTION_SHADOWMAP "direction shadow map"
#define GBUFFER_TRANSMITTANCE_LUT "TransmittanceLUT"
#define GBUFFER_MULTISCATTER_LUT "MultiScatterLUT"
#define GBUFFER_ATMOSPHERE "Atmosphere"
#define GBUFFER_DIRECT_LIGHT "direct light"
#define GBUFFER_VOXEL_VALIZATION "voxelValizationTexture"
#define GBUFFER_VOXEL_DEPTH "voxel depth"
#define GBUFFER_VXGT_COLOR "vxgi color"
#define GBUFFER_VXGT_REFLECT_COLOR "vxgi reflect color"

namespace Marbas::Job {

RenderGraphJob::RenderGraphJob(RHIFactory* rhiFactory, std::shared_ptr<RenderGraph>& graph,
                               std::shared_ptr<RenderGraph>& precomputeGraph,
                               std::shared_ptr<RenderGraphResourceManager> resMgr)
    : m_rhiFactory(rhiFactory), m_resMgr(resMgr), m_renderGraph(graph), m_precomputeRenderGraph(precomputeGraph) {
  m_precomputeFence = rhiFactory->CreateFence();

  LOG(INFO) << "create render graph resource";
  CreateRenderGraphResource();

  LOG(INFO) << "create render graph pass";
  CreateRenderGraphPass();

  m_precomputeRenderGraph->Compile();
  m_renderGraph->Compile();

  /**
   * precompute all pass
   */
  m_precomputeRenderGraph->Execute(nullptr, nullptr, nullptr, m_precomputeFence);
  m_rhiFactory->WaitForFence(m_precomputeFence);
  m_rhiFactory->ResetFence(m_precomputeFence);
  LOG(INFO) << "execute the precompute render graph.";
}

RenderGraphJob::~RenderGraphJob() { m_rhiFactory->DestroyFence(m_precomputeFence); }

void
RenderGraphJob::init() {}

void
RenderGraphJob::update(DeltaTime deltaTime, void* data) {
  auto* renderInfo = reinterpret_cast<RenderInfo*>(data);
  auto* userData = reinterpret_cast<RenderUserData*>(renderInfo->userData);

  auto* waitSemaphore = renderInfo->waitSemaphore;
  auto* signalSemaphore = renderInfo->signalSemaphore;
  auto* fence = renderInfo->fence;
  m_renderGraph->Execute(waitSemaphore, signalSemaphore, fence, userData);
}

void
RenderGraphJob::CreateRenderGraphResource() {
  // TODO: how to change it dynamically
  uint32_t width = 1920;
  uint32_t height = 1080;

  ImageCreateInfo createInfo;

  // geometry pass
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = width;
  createInfo.height = height;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  m_resMgr->CreateTexture(GBUFFER_NORMAL, createInfo);
  m_resMgr->CreateTexture(GBUFFER_POSITION, createInfo);

  createInfo.format = ImageFormat::RGBA;
  m_resMgr->CreateTexture(GBUFFER_DIFFUSE, createInfo);

  createInfo.format = ImageFormat::DEPTH;
  createInfo.usage = ImageUsageFlags::DEPTH_STENCIL | ImageUsageFlags::SHADER_READ;
  m_resMgr->CreateTexture(GBUFFER_DEPTH, createInfo);

  // ssao pass
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = width;
  createInfo.height = height;
  createInfo.format = ImageFormat::R32F;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  m_resMgr->CreateTexture(GBUFFER_SSAO, createInfo);

  // directional light shadow map pass
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::DEPTH_STENCIL;
  createInfo.format = ImageFormat::DEPTH;
  createInfo.mipMapLevel = 1;
  createInfo.imageDesc = Image2DArrayDesc{.arraySize = DirectionShadowComponent::splitCount + 1};
  createInfo.height = DirectionShadowComponent::MAX_SHADOWMAP_SIZE;
  createInfo.width = DirectionShadowComponent::MAX_SHADOWMAP_SIZE;
  m_resMgr->CreateTexture(GBUFFER_DIRECTION_SHADOWMAP, createInfo);

  // transmittanceLUT
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = 256;
  createInfo.height = 64;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  m_resMgr->CreateTexture(GBUFFER_TRANSMITTANCE_LUT, createInfo);

  // multiscatterLUT
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = 32;
  createInfo.height = 32;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  m_resMgr->CreateTexture(GBUFFER_MULTISCATTER_LUT, createInfo);

  // atmosphere
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = 256;
  createInfo.height = 128;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  m_resMgr->CreateTexture(GBUFFER_ATMOSPHERE, createInfo);

  // direct light pass
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.format = ImageFormat::RGBA;
  createInfo.mipMapLevel = 1;
  createInfo.imageDesc = Image2DDesc{};
  createInfo.width = width;
  createInfo.height = height;
  m_resMgr->CreateTexture(GBUFFER_DIRECT_LIGHT, createInfo);

  // voxel visualization
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.mipMapLevel = 1;
  createInfo.imageDesc = Image2DDesc{};
  createInfo.width = width;
  createInfo.height = height;
  m_resMgr->CreateTexture(GBUFFER_VOXEL_VALIZATION, createInfo);

  createInfo.format = ImageFormat::DEPTH;
  createInfo.usage = ImageUsageFlags::DEPTH_STENCIL | ImageUsageFlags::SHADER_READ;
  m_resMgr->CreateTexture(GBUFFER_VOXEL_DEPTH, createInfo);

  // voxel color
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = width;
  createInfo.height = height;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  m_resMgr->CreateTexture(GBUFFER_VXGT_COLOR, createInfo);
  m_resMgr->CreateTexture(GBUFFER_VXGT_REFLECT_COLOR, createInfo);
}

void
RenderGraphJob::CreateRenderGraphPass() {
  uint32_t width = 1920;
  uint32_t height = 1080;

  // precompute render pass
  TransmittanceLUTPassCreateInfo transmittanceLUTCreateInfo;
  transmittanceLUTCreateInfo.width = 256;
  transmittanceLUTCreateInfo.height = 64;
  transmittanceLUTCreateInfo.rhiFactory = m_rhiFactory;
  transmittanceLUTCreateInfo.lutTexture = m_resMgr->GetHandler(GBUFFER_TRANSMITTANCE_LUT);
  m_precomputeRenderGraph->AddPass<TransmittanceLUTPass>("TransmittanceLUTPass", transmittanceLUTCreateInfo);

  MultiScatterLUTCreateInfo multiScatterLUTCreateInfo;
  multiScatterLUTCreateInfo.rhiFactory = m_rhiFactory;
  multiScatterLUTCreateInfo.height = 32;
  multiScatterLUTCreateInfo.width = 32;
  multiScatterLUTCreateInfo.transmittanceLUT = m_resMgr->GetHandler(GBUFFER_TRANSMITTANCE_LUT);
  multiScatterLUTCreateInfo.multiScatterLUT = m_resMgr->GetHandler(GBUFFER_MULTISCATTER_LUT);
  m_precomputeRenderGraph->AddPass<MultiScatterLUT>("MultiScatterLUTPass", multiScatterLUTCreateInfo);

  // render pass
  GeometryPassCreateInfo geometryPassCreateInfo;
  geometryPassCreateInfo.height = height;
  geometryPassCreateInfo.width = width;
  geometryPassCreateInfo.rhiFactory = m_rhiFactory;
  geometryPassCreateInfo.normalMetallicTexture = m_resMgr->GetHandler(GBUFFER_NORMAL);
  geometryPassCreateInfo.positionRoughnessTexture = m_resMgr->GetHandler(GBUFFER_POSITION);
  geometryPassCreateInfo.colorTexture = m_resMgr->GetHandler(GBUFFER_DIFFUSE);
  geometryPassCreateInfo.depthTexture = m_resMgr->GetHandler(GBUFFER_DEPTH);
  m_renderGraph->AddPass<GeometryPass>("GeometryPass", geometryPassCreateInfo);

  DirectionShadowMapPassCreateInfo directShadowMapCreateInfo;
  directShadowMapCreateInfo.rhiFactory = m_rhiFactory;
  directShadowMapCreateInfo.directionalShadowMap = m_resMgr->GetHandler(GBUFFER_DIRECTION_SHADOWMAP);
  m_renderGraph->AddPass<DirectionShadowMapPass>("DirectionShadowMap", directShadowMapCreateInfo);

  GI::VoxelizationCreateInfo voxelizationCreateInfo;
  voxelizationCreateInfo.rhiFactory = m_rhiFactory;
  // voxelizationCreateInfo.voxelScene = m_resMgr->GetHandler(GBUFFER_VOXEL_SCENE);
  voxelizationCreateInfo.shadowMap = m_resMgr->GetHandler(GBUFFER_DIRECTION_SHADOWMAP);
  m_renderGraph->AddPass<GI::VoxelizationPass>("VXGIPass", voxelizationCreateInfo);

  GI::LightInjectPassCreateInfo lightInjectCreateInfo;
  lightInjectCreateInfo.rhiFactory = m_rhiFactory;
  m_renderGraph->AddPass<GI::LightInjectPass>("lightInjectPass", lightInjectCreateInfo);

  GI::VXGIPassCreateInfo vxgiCreateInfo;
  vxgiCreateInfo.m_rhiFactory = m_rhiFactory;
  vxgiCreateInfo.m_widht = width;
  vxgiCreateInfo.m_height = height;
  vxgiCreateInfo.m_positionRoughnessTexture = m_resMgr->GetHandler(GBUFFER_POSITION);
  vxgiCreateInfo.m_diffuseTexture = m_resMgr->GetHandler(GBUFFER_DIFFUSE);
  vxgiCreateInfo.m_normalMetallicTexture = m_resMgr->GetHandler(GBUFFER_NORMAL);
  vxgiCreateInfo.m_finalTexture = m_resMgr->GetHandler(GBUFFER_VXGT_COLOR);
  vxgiCreateInfo.m_reflectTexture = m_resMgr->GetHandler(GBUFFER_VXGT_REFLECT_COLOR);
  m_renderGraph->AddPass<GI::VXGIPass>("VXGIPass", vxgiCreateInfo);

  DirectLightPassCreateInfo directLightPassCreateInfo;
  directLightPassCreateInfo.rhiFactory = m_rhiFactory;
  directLightPassCreateInfo.width = width;
  directLightPassCreateInfo.height = height;
  directLightPassCreateInfo.aoTeture = m_resMgr->GetHandler(GBUFFER_SSAO);
  directLightPassCreateInfo.normalTeture = m_resMgr->GetHandler(GBUFFER_NORMAL);
  directLightPassCreateInfo.positionTeture = m_resMgr->GetHandler(GBUFFER_POSITION);
  directLightPassCreateInfo.diffuseTexture = m_resMgr->GetHandler(GBUFFER_DIFFUSE);
  directLightPassCreateInfo.directionalShadowmap = m_resMgr->GetHandler(GBUFFER_DIRECTION_SHADOWMAP);
  directLightPassCreateInfo.indirectSpecular = m_resMgr->GetHandler(GBUFFER_VXGT_REFLECT_COLOR);
  directLightPassCreateInfo.indirectDiffuse = m_resMgr->GetHandler(GBUFFER_VXGT_COLOR);
  directLightPassCreateInfo.finalColorTexture = m_resMgr->GetHandler(GBUFFER_DIRECT_LIGHT);
  m_renderGraph->AddPass<DirectLightPass>("direct light pass", directLightPassCreateInfo);

  AtmospherePassCreateInfo atmosphereCreateInfo;
  atmosphereCreateInfo.height = 128;
  atmosphereCreateInfo.width = 256;
  atmosphereCreateInfo.rhiFactory = m_rhiFactory;
  atmosphereCreateInfo.colorTexture = m_resMgr->GetHandler(GBUFFER_ATMOSPHERE);
  atmosphereCreateInfo.multiscatterLUT = m_resMgr->GetHandler(GBUFFER_MULTISCATTER_LUT);
  atmosphereCreateInfo.transmittanceLUT = m_resMgr->GetHandler(GBUFFER_TRANSMITTANCE_LUT);
  m_renderGraph->AddPass<AtmospherePass>("AtmospherePass", atmosphereCreateInfo);

  SkyImagePassCreateInfo skyImageCreateInfo;
  skyImageCreateInfo.rhiFactory = m_rhiFactory;
  skyImageCreateInfo.width = width;
  skyImageCreateInfo.height = height;
  skyImageCreateInfo.finalColorTexture = m_resMgr->GetHandler(GBUFFER_DIRECT_LIGHT);
  skyImageCreateInfo.finalDepthTexture = m_resMgr->GetHandler(GBUFFER_DEPTH);
  skyImageCreateInfo.atmosphereTexture = m_resMgr->GetHandler(GBUFFER_ATMOSPHERE);
  m_renderGraph->AddPass<SkyImagePass>("skyImagePass", skyImageCreateInfo);

  GridRenderPassCreateInfo gridRenderPassCreateInfo;
  gridRenderPassCreateInfo.rhiFactory = m_rhiFactory;
  gridRenderPassCreateInfo.height = height;
  gridRenderPassCreateInfo.width = width;
  gridRenderPassCreateInfo.finalDepthTexture = m_resMgr->GetHandler(GBUFFER_DEPTH);
  gridRenderPassCreateInfo.finalColorTexture = m_resMgr->GetHandler(GBUFFER_DIRECT_LIGHT);
  m_renderGraph->AddPass<GridRenderPass>("gridPass", gridRenderPassCreateInfo);
}

};  // namespace Marbas::Job
