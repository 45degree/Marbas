#include "RenderSystem.hpp"

#include <glog/logging.h>

#include "AssetManager/ModelAsset.hpp"
#include "AssetManager/Singleton.hpp"
#include "Core/Renderer/Pass/AtmospherePass.hpp"
#include "Core/Renderer/Pass/DirectionLightShadowMapPass.hpp"
#include "Core/Renderer/Pass/ForwardPass/GridPass.hpp"
#include "Core/Renderer/Pass/ForwardPass/SkyImagePass.hpp"
#include "Core/Renderer/Pass/GeometryPass.hpp"
#include "Core/Renderer/Pass/PreComputePass/MultiScatterLUT.hpp"
#include "Core/Renderer/Pass/PreComputePass/TransmittanceLUT.hpp"
#include "Core/Renderer/RenderGraph/RenderGraph.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphResourceManager.hpp"
#include "Core/Scene/Component/AABBComponent.hpp"
#include "Core/Scene/Component/EnvironmentComponent.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Component/RenderMeshComponent.hpp"

namespace Marbas {

std::shared_ptr<RenderGraphResourceManager> RenderSystem::s_resourceManager;
std::unique_ptr<RenderGraph> RenderSystem::s_renderGraph;
std::unique_ptr<RenderGraph> RenderSystem::s_precomputeRenderGraph;
Fence* RenderSystem::s_precomputeFence = nullptr;
ImageView* RenderSystem::s_resultImageView = nullptr;
entt::sigh<void(ImageView*)> RenderSystem::s_newResultImageViewEvent;
entt::sink<entt::sigh<void(ImageView*)>> RenderSystem::s_sink = {s_newResultImageViewEvent};

static std::optional<ImageView*> s_lastResultImageView;

struct ScreenSpaceShadowInfo {
  RenderGraphTextureHandler screenSpaceShadowInfo;
};

void
RenderSystem::Initialize(RHIFactory* rhiFactory) {
  /**
   * create all resource
   */
  s_resourceManager = std::make_shared<RenderGraphResourceManager>(rhiFactory);
  s_renderGraph = std::make_unique<RenderGraph>(rhiFactory, s_resourceManager);
  s_precomputeRenderGraph = std::make_unique<RenderGraph>(rhiFactory, s_resourceManager);
  s_precomputeFence = rhiFactory->CreateFence();

  auto& geometryPassCreateInfo = *Singleton<GeometryPassCreateInfo>::GetInstance();
  auto& directShadowMapData = *Singleton<DirectShadowMapData>::GetInstance();
  auto& screenSpaceShadowInfo = *Singleton<ScreenSpaceShadowInfo>::GetInstance();
  auto& atmosphereCreateInfo = *Singleton<AtmospherePassCreateInfo>::GetInstance();
  auto& transmittanceLURCreateInfo = *Singleton<TransmittanceLUTPassCreateInfo>::GetInstance();
  auto& multiscatterLUTCreateInfo = *Singleton<MultiScatterLUTCreateInfo>::GetInstance();

  ImageCreateInfo createInfo;
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = 1920;
  createInfo.height = 1080;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  auto normalTextureHandler = s_resourceManager->CreateTexture("normalTexture", createInfo);
  auto positionTextureHandler = s_resourceManager->CreateTexture("positionTexture", createInfo);

  createInfo.format = ImageFormat::RGBA;
  auto colorTextureHandler = s_resourceManager->CreateTexture("colorTexture", createInfo);

  createInfo.format = ImageFormat::R32F;
  auto aoTextureHandler = s_resourceManager->CreateTexture("aoTexture", createInfo);
  auto roughnessTextureHandler = s_resourceManager->CreateTexture("roughnessTexture", createInfo);
  auto metallicTextureHandler = s_resourceManager->CreateTexture("metallicTexture", createInfo);

  createInfo.format = ImageFormat::DEPTH;
  createInfo.usage = ImageUsageFlags::DEPTH_STENCIL;
  auto depthTexture = s_resourceManager->CreateTexture("depthTexture", createInfo);

  geometryPassCreateInfo.width = 1920;
  geometryPassCreateInfo.height = 1080;
  geometryPassCreateInfo.rhiFactory = rhiFactory;
  geometryPassCreateInfo.aoTexture = aoTextureHandler;
  geometryPassCreateInfo.normalTexture = normalTextureHandler;
  geometryPassCreateInfo.colorTexture = colorTextureHandler;
  geometryPassCreateInfo.positionTexture = positionTextureHandler;
  geometryPassCreateInfo.depthTexture = depthTexture;
  geometryPassCreateInfo.metallicTexture = metallicTextureHandler;
  geometryPassCreateInfo.roughnessTexture = roughnessTextureHandler;

  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::DEPTH_STENCIL;
  createInfo.format = ImageFormat::DEPTH;
  createInfo.mipMapLevel = 1;
  createInfo.imageDesc = Image2DArrayDesc{.arraySize = 5};
  createInfo.height = 1024;
  createInfo.width = 1024;
  directShadowMapData.shadowMapTextureHandler = s_resourceManager->CreateTexture("shadowMap", createInfo);

  createInfo.width = 1920;
  createInfo.height = 1080;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  createInfo.format = ImageFormat::R32F;
  createInfo.usage = ImageUsageFlags::COLOR_RENDER_TARGET | ImageUsageFlags::SHADER_READ;
  screenSpaceShadowInfo.screenSpaceShadowInfo = s_resourceManager->CreateTexture("screenSpaceShadowInfo", createInfo);

  // transmittanceLUT
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = 256;
  createInfo.height = 64;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  transmittanceLURCreateInfo.height = 64;
  transmittanceLURCreateInfo.width = 256;
  transmittanceLURCreateInfo.rhiFactory = rhiFactory;
  transmittanceLURCreateInfo.lutTexture = s_resourceManager->CreateTexture("TransmittanceLUT", createInfo);

  // multiscatterLUT
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = 32;
  createInfo.height = 32;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  multiscatterLUTCreateInfo.height = 32;
  multiscatterLUTCreateInfo.width = 32;
  multiscatterLUTCreateInfo.rhiFactory = rhiFactory;
  multiscatterLUTCreateInfo.multiScatterLUT = s_resourceManager->CreateTexture("MultiScatterLUT", createInfo);
  multiscatterLUTCreateInfo.transmittanceLUT = transmittanceLURCreateInfo.lutTexture;

  // atmosphere
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = 256;
  createInfo.height = 128;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  atmosphereCreateInfo.width = 256;
  atmosphereCreateInfo.height = 128;
  atmosphereCreateInfo.rhiFactory = rhiFactory;
  atmosphereCreateInfo.colorTexture = s_resourceManager->CreateTexture("Atmosphere", createInfo);
  atmosphereCreateInfo.transmittanceLUT = transmittanceLURCreateInfo.lutTexture;
  atmosphereCreateInfo.multiscatterLUT = multiscatterLUTCreateInfo.multiScatterLUT;
}

void
RenderSystem::Destroy(RHIFactory* rhiFactory) {
  Singleton<GeometryPassCreateInfo>::Destroy();
  Singleton<DirectShadowMapData>::Destroy();
  Singleton<ScreenSpaceShadowInfo>::Destroy();
  Singleton<AtmospherePassCreateInfo>::Destroy();
  Singleton<TransmittanceLUTPassCreateInfo>::Destroy();

  rhiFactory->DestroyFence(s_precomputeFence);

  s_renderGraph = nullptr;
  s_precomputeRenderGraph = nullptr;
  s_resourceManager = nullptr;
};

ImageView*
RenderSystem::GetOutputView() {
  return s_resultImageView;
}

void
RenderSystem::Update(const RenderInfo& renderInfo) {
  // select all model need to be rendered
  auto* scene = renderInfo.scene;
  auto& world = scene->GetWorld();
  auto camera = scene->GetEditorCamrea();
  auto& frustum = camera->GetFrustum();

  auto modelView = world.view<ModelSceneNode, AABBComponent, TransformComp>();

  for (auto&& [entity, node, aabb, tran] : modelView.each()) {
    if (!aabb.IsOnFrustum(frustum, tran.GetGlobalTransform())) {
      if (world.any_of<RenderComponent>(entity)) {
        world.remove<RenderComponent>(entity);
      }
    } else {
      if (!world.any_of<RenderComponent>(entity)) {
        world.emplace<RenderComponent>(entity);
      }
    }
  }

  s_renderGraph->Execute(renderInfo.waitSemaphore, renderInfo.signalSemaphore, renderInfo.fence);
}

void
RenderSystem::CreateRenderGraph(Scene* scene, RHIFactory* rhiFactory) {
  /**
   * clear all old pass
   */
  rhiFactory->WaitIdle();
  s_renderGraph->ClearAllPass();
  s_precomputeRenderGraph->ClearAllPass();

  /**
   * create new pass
   */

  auto& geometryPassCreateInfo = *Singleton<GeometryPassCreateInfo>::GetInstance();
  auto& directShadowMapData = *Singleton<DirectShadowMapData>::GetInstance();
  auto& screenSpaceShadowInfo = *Singleton<ScreenSpaceShadowInfo>::GetInstance();
  auto& atmosphereCreateInfo = *Singleton<AtmospherePassCreateInfo>::GetInstance();
  auto& transmittanceLUTCreateInfo = *Singleton<TransmittanceLUTPassCreateInfo>::GetInstance();
  auto& multiScatterLUTCreateInfo = *Singleton<MultiScatterLUTCreateInfo>::GetInstance();

  /**
   * precompute pass
   */
  auto& world = scene->GetWorld();
  auto envView = world.view<EnvironmentComponent>();
  LOG_IF(WARNING, envView.size() != 1) << "find multi environment component, use the first one";

  s_precomputeRenderGraph->AddPass<TransmittanceLUTPass>("TransmittanceLUTPass", transmittanceLUTCreateInfo);
  s_precomputeRenderGraph->AddPass<MultiScatterLUT>("MultiScatterLUTPass", multiScatterLUTCreateInfo);

  // light all probe
  // 每一个probe用一个compute shader计算

  geometryPassCreateInfo.scene = scene;
  s_renderGraph->AddPass<GeometryPass>("geometryPass", geometryPassCreateInfo);

  auto directLightView = world.view<DirectionLightComponent, DirectionShadowComponent>();

  // TODO: 每一个shadowMap都应该保存下来
  for (auto entity : directLightView) {
    s_renderGraph->AddPass<DirectionShadowMapPass>("directionLight", rhiFactory, scene, entity,
                                                   directShadowMapData.shadowMapTextureHandler);
    // m_renderGraph->AddPass("ScreenSpaceShadowInfo", [&](RenderGraphGraphicsBuilder& builder) {
    //   builder.ReadTexture(directShadowMapData.shadowMapTextureHandler);
    //   builder.ReadTexture(geometryPassCreateInfo.positionTexture);
    //   builder.WriteTexture(screenSpaceShadowInfo.screenSpaceShadowInfo);
    //
    //   RenderGraphPipelineCreateInfo createInfo;
    //   createInfo.AddShader();
    //   createInfo.AddShader();
    //   createInfo.SetColorAttachmentsDesc({{
    //       .initAction = AttachmentInitAction::CLEAR,
    //       .finalAction = AttachmentFinalAction::READ,
    //       .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
    //       .format = ImageFormat::R32F,
    //   }});
    //   createInfo.SetPipelineLayout({
    //       {.bindingPoint = 0, .descriptorType = DescriptorType::IMAGE},
    //       {.bindingPoint = 1, .descriptorType = DescriptorType::IMAGE},
    //       {.bindingPoint = 0, .descriptorType = DescriptorType::UNIFORM_BUFFER},
    //   });
    //
    //   builder.SetPipelineInfo(createInfo);
    //   builder.SetFramebufferSize(1920, 1080, 1);
    //   return [=](RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList) {
    //     RenderArgument argument;
    //     argument.BindImage(0, registry.GetRenderBackendTexture(directShadowMapData.shadowMapTextureHandler));
    //
    //     commandList.Begin({{0, 0, 0, 0}});
    //     commandList.BindArgument();
    //     commandList.End();
    //   };
    // });
  }

  /** Religth probe
   * 1. 计算每一个surfel的直接光照颜色
   * screenSpaceLight = 0
   * for(auto light : lights) {
   *   // 计算shadow map
   *   shadowMap = RenderShadomap(light);
   *
   *   // compute shader
   *   for(surfel : surfels) {
   *     surfel.color += computeLight(surfel, light, shadowMap);
   *   }
   *
   *   // 计算屏幕直接光
   *
   * } // 计算直接光
   *
   * 2. relight probe
   * // compute shader
   * for(auto probe : probes) {
   *  probe.color = computeProbe(probe, surfels);
   * }
   *
   * 3. 添加间接光照
   *
   */

  // draw atmosphere
  atmosphereCreateInfo.scene = scene;
  s_renderGraph->AddPass<AtmospherePass>("atmospherePass", atmosphereCreateInfo);

  // render the skybox by image
  SkyImagePassCreateInfo skyImagePassCreateInfo;
  skyImagePassCreateInfo.width = 1920;
  skyImagePassCreateInfo.height = 1080;
  skyImagePassCreateInfo.scene = scene;
  skyImagePassCreateInfo.rhiFactory = rhiFactory;
  skyImagePassCreateInfo.finalDepthTexture = geometryPassCreateInfo.depthTexture;
  skyImagePassCreateInfo.finalColorTexture = geometryPassCreateInfo.colorTexture;
  skyImagePassCreateInfo.atmosphereTexture = atmosphereCreateInfo.colorTexture;
  s_renderGraph->AddPass<SkyImagePass>("skyImagePass", skyImagePassCreateInfo);

  // draw grid
  GridRenderPassCreateInfo gridRenderPassCreateInfo;
  gridRenderPassCreateInfo.finalDepthTexture = geometryPassCreateInfo.depthTexture;
  gridRenderPassCreateInfo.finalColorTexture = geometryPassCreateInfo.colorTexture;
  gridRenderPassCreateInfo.scene = scene;
  gridRenderPassCreateInfo.rhiFactory = rhiFactory;
  gridRenderPassCreateInfo.width = 1920;
  gridRenderPassCreateInfo.height = 1080;
  s_renderGraph->AddPass<GridRenderPass>("gridPass", gridRenderPassCreateInfo);

  s_precomputeRenderGraph->Compile();
  s_renderGraph->Compile();

  s_resultImageView = s_resourceManager->GetImageView(geometryPassCreateInfo.colorTexture);

  /**
   * precompute all pass
   */
  s_precomputeRenderGraph->Execute(nullptr, nullptr, s_precomputeFence);
  rhiFactory->WaitForFence(s_precomputeFence);
  rhiFactory->ResetFence(s_precomputeFence);
  LOG(INFO) << "execute the precompute render graph.";

  /**
   * signal the result
   */
  if (!s_lastResultImageView.has_value() || *s_lastResultImageView != s_resultImageView) {
    s_lastResultImageView = s_resultImageView;
    s_newResultImageViewEvent.publish(s_resultImageView);
  }
}

void
RenderSystem::RerunPreComputePass(const StringView& passName, RHIFactory* rhiFactory) {
  /**
   * precompute all pass
   */
  s_precomputeRenderGraph->ExecuteAlone(passName, nullptr, nullptr, s_precomputeFence);
  rhiFactory->WaitForFence(s_precomputeFence);
  rhiFactory->ResetFence(s_precomputeFence);
  LOG(INFO) << FORMAT("re-run the precompute pass:{}.", passName);
}

}  // namespace Marbas
