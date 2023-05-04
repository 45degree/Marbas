#include "RenderSystem.hpp"

#include <glog/logging.h>

#include "AssetManager/ModelAsset.hpp"
#include "AssetManager/Singleton.hpp"
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
#include "Core/Scene/GPUDataPipeline/LightGPUData.hpp"
#include "Core/Scene/GPUDataPipeline/ModelGPUData.hpp"

namespace Marbas {

std::shared_ptr<RenderGraphResourceManager> RenderSystem::s_resourceManager;
std::unique_ptr<RenderGraph> RenderSystem::s_renderGraph;
std::unique_ptr<RenderGraph> RenderSystem::s_precomputeRenderGraph;
Fence* RenderSystem::s_precomputeFence = nullptr;
ImageView* RenderSystem::s_resultImageView = nullptr;
entt::sigh<void(ImageView*)> RenderSystem::s_newResultImageViewEvent;
entt::sink<entt::sigh<void(ImageView*)>> RenderSystem::s_sink = {s_newResultImageViewEvent};

static std::optional<ImageView*> s_lastResultImageView;

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
  auto& directShadowMapData = *Singleton<DirectionShadowMapPassCreateInfo>::GetInstance();
  auto& atmosphereCreateInfo = *Singleton<AtmospherePassCreateInfo>::GetInstance();
  auto& transmittanceLURCreateInfo = *Singleton<TransmittanceLUTPassCreateInfo>::GetInstance();
  auto& multiscatterLUTCreateInfo = *Singleton<MultiScatterLUTCreateInfo>::GetInstance();
  auto& directLightPassCreateInfo = *Singleton<DirectLightPassCreateInfo>::GetInstance();
  auto& ssaoPassCreateInfo = *Singleton<SSAOCreateInfo>::GetInstance();

  ImageCreateInfo createInfo;
  // geometry pass
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

  createInfo.format = ImageFormat::DEPTH;
  createInfo.usage = ImageUsageFlags::DEPTH_STENCIL | ImageUsageFlags::SHADER_READ;
  auto depthTexture = s_resourceManager->CreateTexture("depthTexture", createInfo);

  geometryPassCreateInfo.width = 1920;
  geometryPassCreateInfo.height = 1080;
  geometryPassCreateInfo.rhiFactory = rhiFactory;
  geometryPassCreateInfo.normalMetallicTexture = normalTextureHandler;
  geometryPassCreateInfo.colorTexture = colorTextureHandler;
  geometryPassCreateInfo.positionRoughnessTexture = positionTextureHandler;
  geometryPassCreateInfo.depthTexture = depthTexture;

  // ssao pass
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = 1920;
  createInfo.height = 1080;
  createInfo.format = ImageFormat::R32F;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  auto ssaoTexture = s_resourceManager->CreateTexture("ssaoTexture", createInfo);

  ssaoPassCreateInfo.normalTexture = normalTextureHandler;
  ssaoPassCreateInfo.posTexture = positionTextureHandler;
  ssaoPassCreateInfo.depthTexture = depthTexture;
  ssaoPassCreateInfo.rhiFactory = rhiFactory;
  ssaoPassCreateInfo.width = 1920;
  ssaoPassCreateInfo.height = 1080;
  ssaoPassCreateInfo.ssaoTexture = ssaoTexture;

  // directional light shadow map pass
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::DEPTH_STENCIL;
  createInfo.format = ImageFormat::DEPTH;
  createInfo.mipMapLevel = 1;
  createInfo.imageDesc = Image2DArrayDesc{.arraySize = DirectionShadowComponent::splitCount + 1};
  createInfo.height = DirectionShadowComponent::MAX_SHADOWMAP_SIZE;
  createInfo.width = DirectionShadowComponent::MAX_SHADOWMAP_SIZE;
  directShadowMapData.rhiFactory = rhiFactory;
  directShadowMapData.directionalShadowMap = s_resourceManager->CreateTexture("shadowMap", createInfo);

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

  // direct light pass
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.format = ImageFormat::RGBA;
  createInfo.mipMapLevel = 1;
  createInfo.imageDesc = Image2DDesc{};
  createInfo.width = 1920;
  createInfo.height = 1080;
  directLightPassCreateInfo.finalColorTexture = s_resourceManager->CreateTexture("finalColorTexture", createInfo);
  directLightPassCreateInfo.width = 1920;
  directLightPassCreateInfo.height = 1080;
  directLightPassCreateInfo.rhiFactory = rhiFactory;
  directLightPassCreateInfo.directionalShadowmap = directShadowMapData.directionalShadowMap;
  directLightPassCreateInfo.aoTeture = ssaoPassCreateInfo.ssaoTexture;
  directLightPassCreateInfo.diffuseTexture = geometryPassCreateInfo.colorTexture;
  directLightPassCreateInfo.normalTeture = geometryPassCreateInfo.normalMetallicTexture;
  directLightPassCreateInfo.positionTeture = geometryPassCreateInfo.positionRoughnessTexture;

  // vsgi voxelization pass
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::STORAGE;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.mipMapLevel = 1;
  createInfo.imageDesc = Image3DDesc{.depth = 512};
  createInfo.width = 512;
  createInfo.height = 512;
  auto voxelSceneTexture = s_resourceManager->CreateTexture("voxelSceneTexture", createInfo);
}

void
RenderSystem::Destroy(RHIFactory* rhiFactory) {
  Singleton<GeometryPassCreateInfo>::Destroy();
  Singleton<SSAOCreateInfo>::Destroy();
  Singleton<DirectionShadowMapPassCreateInfo>::Destroy();
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

  UpdateDirectionShadowInfo(scene);
  UpdateMeshGPUAsset(scene);
  UpdateShadowMapAtlasPosition(scene);
  UpdateLightGPUData(scene);

  s_renderGraph->Execute(scene, renderInfo.waitSemaphore, renderInfo.signalSemaphore, renderInfo.fence);
}

void
RenderSystem::CreateRenderGraph(RHIFactory* rhiFactory) {
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
  auto& directShadowMapData = *Singleton<DirectionShadowMapPassCreateInfo>::GetInstance();
  auto& atmosphereCreateInfo = *Singleton<AtmospherePassCreateInfo>::GetInstance();
  auto& transmittanceLUTCreateInfo = *Singleton<TransmittanceLUTPassCreateInfo>::GetInstance();
  auto& multiScatterLUTCreateInfo = *Singleton<MultiScatterLUTCreateInfo>::GetInstance();
  auto& directLightPassCreateInfo = *Singleton<DirectLightPassCreateInfo>::GetInstance();
  auto& ssaoPassCreateInfo = *Singleton<SSAOCreateInfo>::GetInstance();

  /**
   * precompute pass
   */
  // auto& world = scene->GetWorld();
  // auto envView = world.view<EnvironmentComponent>();
  // LOG_IF(WARNING, envView.size() != 1) << "find multi environment component, use the first one";

  s_precomputeRenderGraph->AddPass<TransmittanceLUTPass>("TransmittanceLUTPass", transmittanceLUTCreateInfo);
  s_precomputeRenderGraph->AddPass<MultiScatterLUT>("MultiScatterLUTPass", multiScatterLUTCreateInfo);

  // light all probe
  // 每一个probe用一个compute shader计算

  s_renderGraph->AddPass<GeometryPass>("GeometryPass", geometryPassCreateInfo);

  s_renderGraph->AddPass<SSAOPass>("SSAO", ssaoPassCreateInfo);

  s_renderGraph->AddPass<DirectionShadowMapPass>("DirectionLight", directShadowMapData);

  s_renderGraph->AddPass<DirectLightPass>("direct light pass", directLightPassCreateInfo);

  // draw atmosphere
  s_renderGraph->AddPass<AtmospherePass>("atmospherePass", atmosphereCreateInfo);

  // render the skybox by image
  SkyImagePassCreateInfo skyImagePassCreateInfo;
  skyImagePassCreateInfo.width = 1920;
  skyImagePassCreateInfo.height = 1080;
  skyImagePassCreateInfo.rhiFactory = rhiFactory;
  skyImagePassCreateInfo.finalDepthTexture = geometryPassCreateInfo.depthTexture;
  skyImagePassCreateInfo.finalColorTexture = directLightPassCreateInfo.finalColorTexture;
  skyImagePassCreateInfo.atmosphereTexture = atmosphereCreateInfo.colorTexture;
  s_renderGraph->AddPass<SkyImagePass>("skyImagePass", skyImagePassCreateInfo);

  // draw grid
  GridRenderPassCreateInfo gridRenderPassCreateInfo;
  gridRenderPassCreateInfo.finalDepthTexture = geometryPassCreateInfo.depthTexture;
  gridRenderPassCreateInfo.finalColorTexture = directLightPassCreateInfo.finalColorTexture;
  gridRenderPassCreateInfo.rhiFactory = rhiFactory;
  gridRenderPassCreateInfo.width = 1920;
  gridRenderPassCreateInfo.height = 1080;
  s_renderGraph->AddPass<GridRenderPass>("gridPass", gridRenderPassCreateInfo);

  s_precomputeRenderGraph->Compile();
  s_renderGraph->Compile();

  s_resultImageView = s_resourceManager->GetImageView(directLightPassCreateInfo.finalColorTexture);

  /**
   * precompute all pass
   */
  s_precomputeRenderGraph->Execute(nullptr, nullptr, nullptr, s_precomputeFence);
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
  s_precomputeRenderGraph->ExecuteAlone(passName, nullptr, nullptr, nullptr, s_precomputeFence);
  rhiFactory->WaitForFence(s_precomputeFence);
  rhiFactory->ResetFence(s_precomputeFence);
  LOG(INFO) << FORMAT("re-run the precompute pass:{}.", passName);
}

void
RenderSystem::UpdateShadowMapAtlasPosition(Scene* scene) {
  auto& world = scene->GetWorld();
  auto view = world.view<DirectionShadowComponent>();

  auto upper_power_of_4 = [](unsigned int x) {
    unsigned int t = 0;
    while (pow(4, t) < x) {
      t++;
    }
    return t;
  };

  auto shadowCount = view.size();
  int gridCount = 1 << upper_power_of_4(shadowCount);

  for (int i = 0; i < shadowCount; i++) {
    int row = i / gridCount;
    int col = i % gridCount;

    float XOffset = static_cast<float>(row) / gridCount;
    float YOffset = static_cast<float>(col) / gridCount;
    auto& comp = view.get<DirectionShadowComponent>(view[i]);
    comp.m_viewport.x = XOffset;
    comp.m_viewport.y = YOffset;
    comp.m_viewport.z = 1.0 / gridCount;
    comp.m_viewport.w = 1.0 / gridCount;
  }
}

void
RenderSystem::UpdateLightGPUData(Scene* scene) {
  auto& world = scene->GetWorld();
  auto lightGPUDataMgr = LightGPUDataManager::GetInstance();

  /**
   * add all new ligth
   */
  auto newView = world.view<NewLightTag>();
  for (auto entity : newView) {
    // handle all directional light
    if (world.any_of<DirectionLightComponent>(entity)) {
      auto& light = world.get<DirectionLightComponent>(entity);
      if (world.any_of<DirectionShadowComponent>(entity)) {
        auto& shadow = world.get<DirectionShadowComponent>(entity);
        lightGPUDataMgr->Create(entity, light, shadow);
      } else {
        lightGPUDataMgr->Create(entity, light);
      }
    }

    // TODO: handle point light and spot light
  }

  for (auto entity : newView) {
    world.remove<NewLightTag>(entity);
  }

  /**
   * update light
   */
  auto updateView = world.view<UpdateLightTag>();
  for (auto entity : updateView) {
    if (world.any_of<DirectionShadowComponent>(entity)) {
      const auto& light = world.get<DirectionLightComponent>(entity);
      if (world.any_of<DirectionShadowComponent>(entity)) {
        const auto& shadow = world.get<DirectionShadowComponent>(entity);
        lightGPUDataMgr->Update(entity, light, shadow);
      } else {
        lightGPUDataMgr->Update(entity, light);
      }
    }

    // TODO: handle point light and spot light
  }
  for (auto entity : updateView) {
    world.remove<UpdateLightTag>(entity);
  }

  /**
   * remove light
   */
  auto deleteView = world.view<DeleteLightTag>();
  for (auto entity : deleteView) {
    if (world.any_of<DirectionShadowComponent>(entity)) {
      auto& light = world.get<DirectionShadowComponent>(entity);
      if (world.any_of<DirectionShadowComponent>(entity)) {
        auto& shadow = world.get<DirectionShadowComponent>(entity);
        // TODO: delete entity gpu data with light and shadow and delete the two component
      } else {
        // TODO: delete entity gpu data with light and delete the two component
      }
    }

    // TODO: handle point light and spot light
  }
  for (auto entity : deleteView) {
    world.remove<DeleteLightTag>(entity);
  }
}

void
RenderSystem::UpdateMeshGPUAsset(Scene* scene) {
  auto& world = scene->GetWorld();
  auto modelNodeView = world.view<ModelSceneNode>();
  auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();
  auto modelGPUAssetMgr = ModelGPUDataManager::GetInstance();

  for (auto [entity, node] : modelNodeView.each()) {
    if (node.modelPath == "res://") continue;
    if (!modelAssetMgr->Existed(node.modelPath)) continue;

    /**
     * sync the gpu data
     */
    auto modelAsset = modelAssetMgr->Get(node.modelPath);
    if (!modelGPUAssetMgr->Existed(*modelAsset)) continue;
    modelGPUAssetMgr->Update(*modelAsset);

    /**
     * clear flags
     */
    int meshCount = modelAsset->GetMeshCount();
    for (int i = 0; i < meshCount; i++) {
      modelAsset->GetMesh(i).m_materialTexChanged = false;
      modelAsset->GetMesh(i).m_materialValueChanged = false;
    }
  }
}

void
RenderSystem::UpdateDirectionShadowInfo(Scene* scene) {
  auto& world = scene->GetWorld();
  auto camera = scene->GetEditorCamrea();
  auto view = world.view<DirectionShadowComponent, DirectionLightComponent>();
  for (auto&& [entity, shadow, light] : view.each()) {
    const auto& dir = light.m_direction;
    world.patch<DirectionShadowComponent>(entity, [&dir, &camera](auto& node) {
      node.UpdateShadowInfo(dir, *camera);
      return;
    });
  }
}

}  // namespace Marbas
