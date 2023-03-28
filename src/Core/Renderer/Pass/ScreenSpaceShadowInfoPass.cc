#include "ScreenSpaceShadowInfoPass.hpp"

#include "Core/Scene/Component/ShadowComponent.hpp"

namespace Marbas {

ScreenSpaceShadowInfoPass::ScreenSpaceShadowInfoPass(entt::entity light, RHIFactory* rhiFactory, Scene* scene)
    : m_light(light), m_rhiFactory(rhiFactory), m_scene(scene) {
  auto pipelineContext = m_rhiFactory->GetPipelineContext();
  auto bufferContext = m_rhiFactory->GetBufferContext();
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

  m_sampler = pipelineContext->CreateSampler(samplerCreateInfo);
  m_lightInfoBuffer = bufferContext->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_lightInfo, sizeof(LightInfo), false);
}

ScreenSpaceShadowInfoPass::~ScreenSpaceShadowInfoPass() {}

void
ScreenSpaceShadowInfoPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.ReadTexture(m_shadowMapTexture);
  builder.ReadTexture(m_positionTexture);
  builder.WriteTexture(m_screenSpaceShadowInfoTexture);

  RenderGraphPipelineCreateInfo createInfo;
  createInfo.AddShader(ShaderType::VERTEX_SHADER, "Shader/");
  createInfo.AddShader(ShaderType::FRAGMENT_SHADER, "Shader/");
  createInfo.SetPipelineLayout({
      {.bindingPoint = 0, .descriptorType = DescriptorType::IMAGE},
      {.bindingPoint = 1, .descriptorType = DescriptorType::IMAGE},
      {.bindingPoint = 0, .descriptorType = DescriptorType::UNIFORM_BUFFER},
  });
  createInfo.SetColorAttachmentsDesc({ColorTargetDesc{
      .initAction = AttachmentInitAction::CLEAR,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
      .sampleCount = SampleCount::BIT1,
      .format = ImageFormat::R32F,
  }});

  builder.SetFramebufferSize(1920, 1080, 1);
}

void
ScreenSpaceShadowInfoPass::Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList) {
  auto positionTexture = registry.GetRenderBackendTextureSubResource(m_positionTexture, 0, 1, 0, 1);
  auto shadowMapTexture = registry.GetRenderBackendTextureSubResource(m_shadowMapTexture, 0, 5, 0, 1);

  auto& world = m_scene->GetWorld();
  auto* bufCtx = m_rhiFactory->GetBufferContext();
  auto* pipelineCtx = m_rhiFactory->GetPipelineContext();

  commandList.SetDescriptorSetCount(1);

  auto& shadowComponent = world.get<DirectionShadowComponent>(m_light);
  for (int i = 0; i < shadowComponent.lightSpaceMatrices.size(); i++) {
    m_lightInfo.m_lightMatrix[i] = shadowComponent.lightSpaceMatrices[i];
  }
  bufCtx->UpdateBuffer(m_lightInfoBuffer, &m_lightInfo, sizeof(LightInfo), 0);

  RenderArgument argument;
  argument.BindImage(0, m_sampler, positionTexture);
  argument.BindImage(1, m_sampler, shadowMapTexture);
  argument.BindUniformBuffer(0, m_lightInfoBuffer);

  commandList.Begin({{0, 0, 0, 0}});
  commandList.BindArgument(argument);
  commandList.Draw(6, 1, 0, 0);
  commandList.End();
}

}  // namespace Marbas
