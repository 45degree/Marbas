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
#include "Core/Scene/GPUDataPipeline/LightGPUData.hpp"
#include "Core/Scene/GPUDataPipeline/MeshGPUData.hpp"

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
  uint32_t width = 800;
  uint32_t height = 600;

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
  auto& vxgiPassCreateInfo = *Singleton<GI::VoxelizationCreateInfo>::GetInstance();
  auto& skyImagePassCreateInfo = *Singleton<SkyImagePassCreateInfo>::GetInstance();
  auto& gridRenderPassCreateInfo = *Singleton<GridRenderPassCreateInfo>::GetInstance();
  auto& voxelVolizationCreateInfo = *Singleton<GI::VoxelVisulzationPassCreateInfo>::GetInstance();
  auto& vxgiColorPassCreateInfo = *Singleton<GI::VXGIPassCreateInfo>::GetInstance();
  auto& lightInjectPassCreateInfo = *Singleton<GI::LightInjectPassCreateInfo>::GetInstance();

  ImageCreateInfo createInfo;
  // geometry pass
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = width;
  createInfo.height = height;
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

  geometryPassCreateInfo.width = width;
  geometryPassCreateInfo.height = height;
  geometryPassCreateInfo.rhiFactory = rhiFactory;
  geometryPassCreateInfo.normalMetallicTexture = normalTextureHandler;
  geometryPassCreateInfo.colorTexture = colorTextureHandler;
  geometryPassCreateInfo.positionRoughnessTexture = positionTextureHandler;
  geometryPassCreateInfo.depthTexture = depthTexture;

  // ssao pass
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = width;
  createInfo.height = height;
  createInfo.format = ImageFormat::R32F;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  auto ssaoTexture = s_resourceManager->CreateTexture("ssaoTexture", createInfo);

  ssaoPassCreateInfo.normalTexture = normalTextureHandler;
  ssaoPassCreateInfo.posTexture = positionTextureHandler;
  ssaoPassCreateInfo.depthTexture = depthTexture;
  ssaoPassCreateInfo.rhiFactory = rhiFactory;
  ssaoPassCreateInfo.width = width;
  ssaoPassCreateInfo.height = height;
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

  // vxgi pass
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::STORAGE | ImageUsageFlags::SHADER_READ;
  createInfo.width = 256;
  createInfo.height = 256;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.imageDesc = Image3DDesc{.depth = 256};
  createInfo.mipMapLevel = 1;
  auto voxelSceneTexture = s_resourceManager->CreateTexture("voxelSceneTexture", createInfo);
  vxgiPassCreateInfo.shadowMap = directShadowMapData.directionalShadowMap;
  vxgiPassCreateInfo.rhiFactory = rhiFactory;
  vxgiPassCreateInfo.voxelScene = voxelSceneTexture;

  // voxel normal
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::STORAGE | ImageUsageFlags::SHADER_READ;
  createInfo.width = 256;
  createInfo.height = 256;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.imageDesc = Image3DDesc{.depth = 256};
  createInfo.mipMapLevel = 1;
  auto voxelNormalTexture = s_resourceManager->CreateTexture("voxelNormalTexture", createInfo);

  // voxel Radiance
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::STORAGE | ImageUsageFlags::SHADER_READ;
  createInfo.width = 256;
  createInfo.height = 256;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.imageDesc = Image3DDesc{.depth = 256};
  createInfo.mipMapLevel = static_cast<uint32_t>(std::floor(std::log2(256))) + 1;
  auto voxelRadianceTexture = s_resourceManager->CreateTexture("voxelRadianceTexture", createInfo);
  lightInjectPassCreateInfo.rhiFactory = rhiFactory;
  lightInjectPassCreateInfo.voxelTexture = voxelSceneTexture;
  lightInjectPassCreateInfo.voxelNormal = voxelNormalTexture;
  lightInjectPassCreateInfo.voxelRadiance = voxelRadianceTexture;

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
  createInfo.width = width;
  createInfo.height = height;
  directLightPassCreateInfo.finalColorTexture = s_resourceManager->CreateTexture("finalColorTexture", createInfo);
  directLightPassCreateInfo.width = width;
  directLightPassCreateInfo.height = height;
  directLightPassCreateInfo.rhiFactory = rhiFactory;
  directLightPassCreateInfo.directionalShadowmap = directShadowMapData.directionalShadowMap;
  directLightPassCreateInfo.aoTeture = ssaoPassCreateInfo.ssaoTexture;
  directLightPassCreateInfo.diffuseTexture = geometryPassCreateInfo.colorTexture;
  directLightPassCreateInfo.normalTeture = geometryPassCreateInfo.normalMetallicTexture;
  directLightPassCreateInfo.positionTeture = geometryPassCreateInfo.positionRoughnessTexture;

  // sky image pass
  skyImagePassCreateInfo.width = width;
  skyImagePassCreateInfo.height = height;
  skyImagePassCreateInfo.rhiFactory = rhiFactory;
  skyImagePassCreateInfo.finalDepthTexture = geometryPassCreateInfo.depthTexture;
  skyImagePassCreateInfo.finalColorTexture = directLightPassCreateInfo.finalColorTexture;
  skyImagePassCreateInfo.atmosphereTexture = atmosphereCreateInfo.colorTexture;

  // grid pass
  gridRenderPassCreateInfo.finalDepthTexture = geometryPassCreateInfo.depthTexture;
  gridRenderPassCreateInfo.finalColorTexture = directLightPassCreateInfo.finalColorTexture;
  gridRenderPassCreateInfo.rhiFactory = rhiFactory;
  gridRenderPassCreateInfo.width = width;
  gridRenderPassCreateInfo.height = height;

  // voxel visualization
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.mipMapLevel = 1;
  createInfo.imageDesc = Image2DDesc{};
  createInfo.width = width;
  createInfo.height = height;
  voxelVolizationCreateInfo.m_resultTexture = s_resourceManager->CreateTexture("voxelValizationTexture", createInfo);

  createInfo.format = ImageFormat::DEPTH;
  createInfo.usage = ImageUsageFlags::DEPTH_STENCIL | ImageUsageFlags::SHADER_READ;
  voxelVolizationCreateInfo.m_depthTexture = s_resourceManager->CreateTexture("voxelDepth", createInfo);
  voxelVolizationCreateInfo.m_voxelTexture = voxelRadianceTexture;
  voxelVolizationCreateInfo.m_width = width;
  voxelVolizationCreateInfo.m_height = height;
  voxelVolizationCreateInfo.rhiFactory = rhiFactory;

  // voxel color
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = width;
  createInfo.height = height;
  createInfo.format = ImageFormat::RGBA32F;
  createInfo.imageDesc = Image2DDesc();
  createInfo.mipMapLevel = 1;
  vxgiColorPassCreateInfo.m_finalTexture = s_resourceManager->CreateTexture("vxgiColor", createInfo);
  vxgiColorPassCreateInfo.m_voxelTexture = voxelRadianceTexture;
  vxgiColorPassCreateInfo.m_diffuseTexture = geometryPassCreateInfo.colorTexture;
  vxgiColorPassCreateInfo.m_positionRoughnessTexture = positionTextureHandler;
  vxgiColorPassCreateInfo.m_normalMetallicTexture = normalTextureHandler;
  vxgiColorPassCreateInfo.m_rhiFactory = rhiFactory;
  vxgiColorPassCreateInfo.m_height = height;
  vxgiColorPassCreateInfo.m_widht = width;
}

void
RenderSystem::Destroy(RHIFactory* rhiFactory) {
  Singleton<GeometryPassCreateInfo>::Destroy();
  Singleton<SSAOCreateInfo>::Destroy();
  Singleton<DirectionShadowMapPassCreateInfo>::Destroy();
  Singleton<AtmospherePassCreateInfo>::Destroy();
  Singleton<TransmittanceLUTPassCreateInfo>::Destroy();
  Singleton<SkyImagePassCreateInfo>::Destroy();
  Singleton<GridRenderPassCreateInfo>::Destroy();
  Singleton<GI::VoxelizationCreateInfo>::Destroy();
  Singleton<GI::VoxelVisulzationPassCreateInfo>::Destroy();
  Singleton<GI::VXGIPassCreateInfo>::Destroy();
  Singleton<GI::LightInjectPassCreateInfo>::Destroy();

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
      if (world.any_of<RenderableTag>(entity)) {
        world.remove<RenderableTag>(entity);
      }
    } else {
      if (!world.any_of<RenderableTag>(entity)) {
        world.emplace<RenderableTag>(entity);
      }
    }
  }

  UpdateDirectionShadowInfo(scene);
  UpdateMeshGPUAsset(scene).start([](auto&&) {});
  UpdateShadowMapAtlasPosition(scene);
  UpdateLightGPUData(scene).start([](auto&&) {});

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
  auto& vxgiPassCreateInfo = *Singleton<GI::VoxelizationCreateInfo>::GetInstance();
  auto& skyImagePassCreateInfo = *Singleton<SkyImagePassCreateInfo>::GetInstance();
  auto& gridRenderPassCreateInfo = *Singleton<GridRenderPassCreateInfo>::GetInstance();
  auto& voxelVolizationCreateInfo = *Singleton<GI::VoxelVisulzationPassCreateInfo>::GetInstance();
  auto& vxgiColorPassCreateInfo = *Singleton<GI::VXGIPassCreateInfo>::GetInstance();
  auto& lightInjectPassCreateInfo = *Singleton<GI::LightInjectPassCreateInfo>::GetInstance();

  /**
   * precompute pass
   */

  s_precomputeRenderGraph->AddPass<TransmittanceLUTPass>("TransmittanceLUTPass", transmittanceLUTCreateInfo);
  s_precomputeRenderGraph->AddPass<MultiScatterLUT>("MultiScatterLUTPass", multiScatterLUTCreateInfo);

  /**
   * render graph
   */
  s_renderGraph->AddPass<GeometryPass>("GeometryPass", geometryPassCreateInfo);
  // s_renderGraph->AddPass<SSAOPass>("SSAO", ssaoPassCreateInfo);
  s_renderGraph->AddPass<DirectionShadowMapPass>("DirectionLight", directShadowMapData);
  s_renderGraph->AddPass<GI::VoxelizationPass>("VXGIPass", vxgiPassCreateInfo);
  s_renderGraph->AddPass<GI::LightInjectPass>("lightInjectPass", lightInjectPassCreateInfo);
  s_renderGraph->AddPass<GI::VXGIPass>("VXGIPass", vxgiColorPassCreateInfo);
  s_renderGraph->AddPass<GI::VoxelVisulzationPass>("VXGIPass", voxelVolizationCreateInfo);
  s_renderGraph->AddPass<DirectLightPass>("direct light pass", directLightPassCreateInfo);
  s_renderGraph->AddPass<AtmospherePass>("atmospherePass", atmosphereCreateInfo);
  s_renderGraph->AddPass<SkyImagePass>("skyImagePass", skyImagePassCreateInfo);
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

Task<>
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
        co_await lightGPUDataMgr->CreateAsync(entity, light, shadow);
      } else {
        co_await lightGPUDataMgr->CreateAsync(entity, light);
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
        co_await lightGPUDataMgr->UpdateAsync(entity, light, shadow);
      } else {
        co_await lightGPUDataMgr->UpdateAsync(entity, light);
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

Task<void>
RenderSystem::UpdateMeshGPUAsset(Scene* scene) {
  auto& world = scene->GetWorld();
  auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();
  auto meshGPUDataManager = MeshGPUDataManager::GetInstance();

  // find all renderable model, and update the gpu mesh data
  auto modelNodeView = world.view<ModelSceneNode, RenderableTag>();
  for (auto&& [model, node] : modelNodeView.each()) {
    for (auto meshEntity : node.m_meshEntities) {
      if (!world.any_of<MeshComponent>(meshEntity)) continue;

      auto& meshComponent = world.get<MeshComponent>(meshEntity);
      auto& index = meshComponent.index;
      auto& mesh = meshComponent.m_modelAsset->GetMesh(index);

      if (!meshGPUDataManager->Existed(meshEntity)) {
        co_await meshGPUDataManager->CreateAsync(meshEntity, mesh);
      } else {
        co_await meshGPUDataManager->UpdateAsync(meshEntity, mesh);
      }

      /**
       * clear flags
       */
      mesh.m_materialTexChanged = false;
      mesh.m_materialValueChanged = false;

      if (!world.any_of<RenderableMeshTag>(meshEntity)) {
        world.emplace<RenderableMeshTag>(meshEntity);
      }
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
