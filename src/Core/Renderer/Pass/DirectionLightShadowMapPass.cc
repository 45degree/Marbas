#include "DirectionLightShadowMapPass.hpp"

#include "Common/MathCommon.hpp"
#include "Core/Common.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Component/RenderMeshComponent.hpp"
#include "Core/Scene/Component/ShadowComponent.hpp"

namespace Marbas {

void
DirectionShadowMapPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.WriteTexture(m_shadowMapTextureHandler, TextureAttachmentType::DEPTH, 0, 5);

  RenderGraphPipelineCreateInfo createInfo;
  createInfo.SetPipelineLayout({
      {.bindingPoint = 0, .descriptorType = DescriptorType::UNIFORM_BUFFER},
      {.bindingPoint = 1, .descriptorType = DescriptorType::UNIFORM_BUFFER},
  });
  createInfo.SetMultiSamples(SampleCount::BIT1);
  createInfo.SetDepthAttachmentDesc({
      .initAction = AttachmentInitAction::CLEAR,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::DEPTH_STENCIL | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
  });
  createInfo.AddShader(ShaderType::VERTEX_SHADER, "Shader/directionLightShadowMap.vert.glsl.spv");
  createInfo.AddShader(ShaderType::GEOMETRY_SHADER, "Shader/directionLightShadowMap.geom.glsl.spv");
  createInfo.AddShader(ShaderType::FRAGMENT_SHADER, "Shader/directionLightShadowMap.frag.glsl.spv");
  createInfo.SetVertexInputElementDesc(GetMeshVertexInfoLayout());
  createInfo.SetVertexInputElementView(GetMeshVertexViewInfo());

  DepthStencilCreateInfo depthStencilCreateInfo;
  depthStencilCreateInfo.depthTestEnable = true;
  createInfo.SetDepthStencil(depthStencilCreateInfo);
  builder.SetPipelineInfo(createInfo);

  builder.SetFramebufferSize(m_width, m_height, 5);
}

void
DirectionShadowMapPass::Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList) {
  // Create Shadow Alias

  // auto& world = m_scene->GetWorld();
  // auto* bufCtx = m_rhiFactory->GetBufferContext();
  // auto* pipelineCtx = m_rhiFactory->GetPipelineContext();
  //
  // auto& shadowComponent = world.get<DirectionShadowComponent>(m_light);
  // for (int i = 0; i < 5 && i < shadowComponent.lightSpaceMatrices.size(); i++) {
  //   m_lightShadowInfo.lightSpaceMatrices[i] = shadowComponent.lightSpaceMatrices[i];
  // }
  // m_lightShadowInfo.lightSpaceMatricesCount = std::min<size_t>(5, shadowComponent.lightSpaceMatrices.size());
  // bufCtx->UpdateBuffer(m_lightShadowInfoBuffer, &m_lightShadowInfo, sizeof(LightShadowInfo), 0);
  //
  // auto view = world.view<RenderMeshComponent>();
  // commandList.SetDescriptorSetCount(view.size());
  //
  // // render all meshes
  // commandList.Begin({{1, 1}});
  // for (auto&& [entity, meshComponent] : view.each()) {
  //   auto& indexCount = meshComponent.indexCount;
  //
  //   RenderArgument argument;
  //   argument.BindUniformBuffer(0, meshComponent.infoBuffer);
  //   argument.BindUniformBuffer(1, m_lightShadowInfoBuffer);
  //
  //   auto& model = meshComponent.meshData.model;
  //   commandList.BindVertexBuffer(meshComponent.vertexBuffer);
  //   commandList.BindIndexBuffer(meshComponent.indexBuffer);
  //   commandList.BindArgument(argument);
  //   commandList.DrawIndex(indexCount, 1, 0, 0, 0);
  // }
  // commandList.End();
}

}  // namespace Marbas
