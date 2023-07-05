#include "LightInjectPass.hpp"

#include <nameof.hpp>

#include "Core/Scene/System/RenderSystemJob/RenderSystem.hpp"

namespace Marbas::GI {

LightInjectPass::LightInjectPass(const LightInjectPassCreateInfo& createInfo) : m_rhiFactory(createInfo.rhiFactory) {}

LightInjectPass::~LightInjectPass() {}

void
LightInjectPass::SetUp(RenderGraphComputeBuilder& builder) {
  auto& lightArgument = LightRenderComponent::GetDescriptorSetArgument();

  builder.BeginPipeline();
  builder.AddShaderArgument(VoxelRenderComponent::GetLightInjectDescriptorArgument());
  builder.AddShaderArgument(lightArgument);
  builder.SetShader("Shader/lightInject.comp.spv");
  builder.EndPipeline();
}

void
LightInjectPass::Execute(RenderGraphComputeRegistry& registry, ComputeCommandBuffer& commandBuffer) {
  auto pipeline = registry.GetPipeline(0);
  auto set = registry.GetInputDescriptorSet();
  auto* userData = reinterpret_cast<Job::RenderUserData*>(registry.GetUserData());
  auto* scene = userData->scene;

  auto renderLightDataView = scene->View<LightRenderComponent>();
  DLOG_IF(WARNING, renderLightDataView.size() > 1)
      << FORMAT("multi {} in the scene", NAMEOF_TYPE(LightRenderComponent));
  auto& renderLightData = scene->Get<LightRenderComponent>(renderLightDataView[0]);
  auto& lightDataSet = renderLightData.m_lightSet;

  auto giDataView = scene->View<VoxelRenderComponent>();

  commandBuffer.Begin();
  for (auto&& [entity, giData] : giDataView.each()) {
    auto& resolution = giData.m_resolution;
    commandBuffer.ClearColor(giData.m_voxelRadiance, {0, 0, 0, 0}, 0, 1, 0, 1);
    commandBuffer.BeginPipeline(pipeline);
    commandBuffer.BindDescriptorSet(pipeline, {giData.m_setForLightInject, lightDataSet});
    commandBuffer.Dispatch(resolution / 8, resolution / 8, resolution / 8);
    commandBuffer.EndPipeline(pipeline);
  }
  commandBuffer.End();
}

bool
LightInjectPass::IsEnable(RenderGraphComputeRegistry& registry) {
  auto* userData = reinterpret_cast<Job::RenderUserData*>(registry.GetUserData());
  auto* scene = userData->scene;

  // no light in the scene
  auto renderLightDataView = scene->View<LightRenderComponent>();
  if (renderLightDataView.size() == 0) return false;

  // no gi probe in the scene
  auto giProbeView = scene->View<VXGIProbeSceneNode>();
  if (giProbeView.size() == 0) return false;

  // no active gi probe in the view port
  auto giDataView = scene->View<VoxelRenderComponent>();
  if (giDataView.size() == 0) return false;

  return true;
}

}  // namespace Marbas::GI
