#include "VoxelizationPass.hpp"

#include <nameof.hpp>

#include "Core/Common.hpp"
#include "Core/Scene/System/RenderSystemJob/RenderSystem.hpp"

namespace Marbas::GI {

VoxelizationPass::VoxelizationPass(const VoxelizationCreateInfo& createInfo)
    : m_shadowMap(createInfo.shadowMap), m_rhiFactory(createInfo.rhiFactory) {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto bufCtx = m_rhiFactory->GetBufferContext();

  SamplerCreateInfo samplerCreateInfo{
      .filter = Marbas::Filter::MIN_MAG_MIP_LINEAR,
      .addressU = Marbas::SamplerAddressMode::WRAP,
      .addressV = Marbas::SamplerAddressMode::WRAP,
      .addressW = Marbas::SamplerAddressMode::WRAP,
      .comparisonOp = Marbas::ComparisonOp::ALWAYS,
      .mipLodBias = 0,
      .minLod = 0,
      .maxLod = 0,
      .borderColor = Marbas::BorderColor::IntOpaqueBlack,
  };
  m_sampler = pipelineCtx->CreateSampler(samplerCreateInfo);
}

VoxelizationPass::~VoxelizationPass() {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  pipelineCtx->DestroySampler(m_sampler);
}

void
VoxelizationPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.ReadTexture(m_shadowMap, m_sampler);

  DescriptorSetArgument m_inputGBufferArgument;
  m_inputGBufferArgument.Bind(0, DescriptorType::IMAGE);  // shadow map

  builder.BeginPipeline();
  builder.AddShaderArgument(m_inputGBufferArgument);
  builder.AddShaderArgument(VoxelRenderComponent::GetVoxelizationDescriptorArgument());
  builder.AddShaderArgument(MeshRenderComponent::GetDescriptorSetArgument());
  builder.AddShaderArgument(LightRenderComponent::GetDescriptorSetArgument());
  builder.SetPushConstantSize(sizeof(glm::mat4));
  builder.EnableDepthTest(false);
  builder.SetCullMode(CullMode::NONE);
  builder.AddShader("Shader/voxelization.vert.spv", ShaderType::VERTEX_SHADER);
  builder.AddShader("Shader/voxelization.geom.spv", ShaderType::GEOMETRY_SHADER);
  builder.AddShader("Shader/voxelization.frag.spv", ShaderType::FRAGMENT_SHADER);
  builder.SetVertexInputElementDesc(GetMeshVertexInfoLayout());
  builder.SetVertexInputElementView(GetMeshVertexViewInfo());
  builder.EndPipeline();

  builder.SetFramebufferSize(256, 256, 1);
}

void
VoxelizationPass::Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandBuffer) {
  auto set = registry.GetInputDescriptorSet();
  auto pipeline = registry.GetPipeline(0);
  auto framebuffer = registry.GetFrameBuffer();

  auto* userData = reinterpret_cast<Job::RenderUserData*>(registry.GetUserData());
  auto* scene = userData->scene;
  auto& world = scene->GetWorld();

  auto renderLightDataView = world.view<LightRenderComponent>();
  DLOG_IF(WARNING, renderLightDataView.size() > 1)
      << FORMAT("multi {} in the scene", NAMEOF_TYPE(LightRenderComponent));
  auto& renderLightData = world.get<LightRenderComponent>(renderLightDataView[0]);
  auto& lightDataSet = renderLightData.m_lightSet;

  auto giDataView = world.view<VoxelRenderComponent>();

  /**
   * record command
   */

  commandBuffer.Begin();

  // clear all new gi data
  // for (auto entity : m_giCreateObserver) {
  //   auto& giData = scene->Get<VoxelRenderComponent>(entity);
  //   commandBuffer.ClearColor(giData.m_voxelStaticDiffuse, {0, 0, 0, 0}, 0, 1, 0, 1);
  //   commandBuffer.ClearColor(giData.m_voxelStaticNormal, {0, 0, 0, 0}, 0, 1, 0, 1);
  // }

  // static model update
  // for (auto&& [entity, giData] : giDataView.each()) {
  //   // if (m_staticModelAddObserver.empty()) continue;
  //   // m_staticModelAddObserver.clear();
  //
  //   commandBuffer.ClearColor(giData.m_voxelStaticDiffuse, {0, 0, 0, 0}, 0, 1, 0, 1);
  //   commandBuffer.ClearColor(giData.m_voxelStaticNormal, {0, 0, 0, 0}, 0, 1, 0, 1);
  //
  //   commandBuffer.BeginPipeline(pipeline, framebuffer, {});
  //   std::array<ViewportInfo, 1> viewportInfo;
  //   viewportInfo[0].x = 0;
  //   viewportInfo[0].y = 0;
  //   viewportInfo[0].width = giData.m_resolution;
  //   viewportInfo[0].height = giData.m_resolution;
  //
  //   std::array<ScissorInfo, 1> scissorInfo;
  //   scissorInfo[0].x = 0;
  //   scissorInfo[0].y = 0;
  //   scissorInfo[0].width = giData.m_resolution;
  //   scissorInfo[0].height = giData.m_resolution;
  //
  //   commandBuffer.SetViewports(viewportInfo);
  //   commandBuffer.SetScissors(scissorInfo);
  //
  //   auto view = scene->View<ModelSceneNode, StaticModelTag, RenderableTag>();
  //   for (auto&& [node, modelNodeComp] : view.each()) {
  //     glm::mat4 modelTransform(1.0);
  //     if (world.any_of<TransformComp>(node)) {
  //       modelTransform = world.get<TransformComp>(node).GetGlobalTransform();
  //     }
  //
  //     commandBuffer.PushConstant(pipeline, &modelTransform, sizeof(glm::mat4), 0);
  //     for (auto mesh : modelNodeComp.m_meshEntities) {
  //       if (!world.any_of<MeshRenderComponent>(mesh)) continue;
  //
  //       auto& meshRenderComponent = world.get<MeshRenderComponent>(mesh);
  //       auto& indexCount = meshRenderComponent.m_indexCount;
  //
  //       commandBuffer.BindDescriptorSet(
  //           pipeline, {set, giData.m_setForStaticVoxelization, meshRenderComponent.m_descriptorSet, lightDataSet});
  //       commandBuffer.BindVertexBuffer(meshRenderComponent.m_vertexBuffer);
  //       commandBuffer.BindIndexBuffer(meshRenderComponent.m_indexBuffer);
  //       commandBuffer.DrawIndexed(meshRenderComponent.m_indexCount, 1, 0, 0, 0);
  //     }
  //   }
  //   commandBuffer.EndPipeline(pipeline);
  //
  //   int resolution = giData.m_resolution;
  //   CopyImageRange srcRange, dstRange;
  //   commandBuffer.CopyImage(giData.m_voxelStaticDiffuse, giData.m_voxelDiffuse, srcRange, dstRange, resolution,
  //                           resolution, resolution);
  //   commandBuffer.CopyImage(giData.m_voxelStaticNormal, giData.m_voxelNormal, srcRange, dstRange, resolution,
  //                           resolution, resolution);
  // }

  for (auto&& [entity, giData] : giDataView.each()) {
    commandBuffer.ClearColor(giData.m_voxelDiffuse, {0, 0, 0, 0}, 0, 1, 0, 1);
    commandBuffer.ClearColor(giData.m_voxelNormal, {0, 0, 0, 0}, 0, 1, 0, 1);
    commandBuffer.BeginPipeline(pipeline, framebuffer, {});
    std::array<ViewportInfo, 1> viewportInfo;
    viewportInfo[0].x = 0;
    viewportInfo[0].y = 0;
    viewportInfo[0].width = giData.m_resolution;
    viewportInfo[0].height = giData.m_resolution;

    std::array<ScissorInfo, 1> scissorInfo;
    scissorInfo[0].x = 0;
    scissorInfo[0].y = 0;
    scissorInfo[0].width = giData.m_resolution;
    scissorInfo[0].height = giData.m_resolution;

    commandBuffer.SetViewports(viewportInfo);
    commandBuffer.SetScissors(scissorInfo);

    auto view = world.view<ModelSceneNode, RenderableTag>();
    for (auto&& [modelNode, modelNodeComp] : view.each()) {
      glm::mat4 modelTransform(1.0);
      if (world.any_of<TransformComp>(modelNode)) {
        modelTransform = world.get<TransformComp>(modelNode).GetGlobalTransform();
      }

      commandBuffer.PushConstant(pipeline, &modelTransform, sizeof(glm::mat4), 0);
      for (auto mesh : modelNodeComp.m_meshEntities) {
        if (!world.any_of<MeshRenderComponent>(mesh)) continue;

        auto& meshRenderComponent = world.get<MeshRenderComponent>(mesh);
        auto& indexCount = meshRenderComponent.m_indexCount;

        commandBuffer.BindDescriptorSet(
            pipeline, {set, giData.m_setForVoxelization, meshRenderComponent.m_descriptorSet, lightDataSet});
        commandBuffer.BindVertexBuffer(meshRenderComponent.m_vertexBuffer);
        commandBuffer.BindIndexBuffer(meshRenderComponent.m_indexBuffer);
        commandBuffer.DrawIndexed(meshRenderComponent.m_indexCount, 1, 0, 0, 0);
      }
    }
    commandBuffer.EndPipeline(pipeline);
  }

  // TODO:
  commandBuffer.End();
}

bool
VoxelizationPass::IsEnable(RenderGraphGraphicsRegistry& registry) {
  auto* userData = reinterpret_cast<Job::RenderUserData*>(registry.GetUserData());
  auto& world = userData->scene->GetWorld();

  // no light in the scene
  auto renderLightDataView = world.view<LightRenderComponent>();
  if (renderLightDataView.size() == 0) return false;

  // no gi probe in the scene
  auto giProbeView = world.view<VXGIProbeSceneNode>();
  if (giProbeView.size() == 0) return false;

  // no active gi probe in the view port
  auto giDataView = world.view<VoxelRenderComponent>();
  if (giDataView.size() == 0) return false;

  // set the observer
  if (userData->changeScene && userData->scene != nullptr) {
    // constexpr auto staticCollector = entt::collector.group<ModelSceneNode, StaticModelTag>()
    //                                      .update<ModelSceneNode>()
    //                                      .where<StaticModelTag>()
    //                                      .update<TransformComp>()
    //                                      .where<StaticModelTag>();
    // constexpr auto giCreateCollector = entt::collector.group<VXGIProbeSceneNode>();
    // userData->scene->ConnectObserve(m_staticModelAddObserver, staticCollector);
    // userData->scene->ConnectObserve(m_giCreateObserver, giCreateCollector);
  }

  return true;
}

};  // namespace Marbas::GI
