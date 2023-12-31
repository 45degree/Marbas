#include "DirectLightPass.hpp"

#include <nameof.hpp>

#include "Core/Scene/Component/Component.hpp"
#include "Core/Scene/System/RenderSystemJob/RenderSystem.hpp"

namespace Marbas {

DirectLightPass::DirectLightPass(const DirectLightPassCreateInfo& createInfo)
    : m_rhiFactory(createInfo.rhiFactory),
      m_width(createInfo.width),
      m_height(createInfo.height),
      m_diffuseTexture(createInfo.diffuseTexture),
      m_normalTexture(createInfo.normalTeture),
      m_positionTexture(createInfo.positionTeture),
      m_roughnessTexture(createInfo.roughnessTeture),
      m_metallicTexture(createInfo.metallicTeture),
      m_aoTexture(createInfo.aoTeture),
      m_indirectDiffuse(createInfo.indirectDiffuse),
      m_indirectSpecular(createInfo.indirectSpecular),
      m_directionalShadowmap(createInfo.directionalShadowmap),
      m_finalColorTexture(createInfo.finalColorTexture) {
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

  m_argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
  m_descriptorSet = pipelineCtx->CreateDescriptorSet(m_argument);

  void* data = &m_cameraInfo;
  auto size = sizeof(CameraInfo);
  m_cameraInfoBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, data, size, false);

  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_descriptorSet,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
      .buffer = m_cameraInfoBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
}

DirectLightPass::~DirectLightPass() {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto bufCtx = m_rhiFactory->GetBufferContext();

  pipelineCtx->DestroySampler(m_sampler);
  pipelineCtx->DestroyDescriptorSet(m_descriptorSet);

  bufCtx->DestroyBuffer(m_cameraInfoBuffer);
}

void
DirectLightPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  constexpr int shadowArraySize = DirectionShadowComponent::shadowMapArraySize;

  builder.ReadTexture(m_diffuseTexture, m_sampler);
  builder.ReadTexture(m_normalTexture, m_sampler);
  builder.ReadTexture(m_positionTexture, m_sampler);
  builder.ReadTexture(m_aoTexture, m_sampler);
  builder.ReadTexture(m_directionalShadowmap, m_sampler, 0, shadowArraySize, 0, 1);
  builder.ReadTexture(m_indirectDiffuse, m_sampler);
  builder.ReadTexture(m_indirectSpecular, m_sampler);
  builder.WriteTexture(m_finalColorTexture);

  builder.SetFramebufferSize(m_width, m_height, 1);

  builder.BeginPipeline();
  builder.EnableDepthTest(false);
  builder.AddColorTarget(ColorTargetDesc{
      .initAction = AttachmentInitAction::CLEAR,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
      .sampleCount = SampleCount::BIT1,
      .format = ImageFormat::RGBA,
  });
  builder.AddBlendAttachments(BlendAttachment{.blendEnable = false});
  builder.AddShader("Shader/ScreenSpace.vert.spv", ShaderType::VERTEX_SHADER);
  builder.AddShader("Shader/directLightPass.frag.spv", ShaderType::FRAGMENT_SHADER);

  DescriptorSetArgument argument;
  argument.Bind(0, DescriptorType::IMAGE);
  argument.Bind(1, DescriptorType::IMAGE);
  argument.Bind(2, DescriptorType::IMAGE);
  argument.Bind(3, DescriptorType::IMAGE);
  argument.Bind(4, DescriptorType::IMAGE);
  argument.Bind(5, DescriptorType::IMAGE);
  argument.Bind(6, DescriptorType::IMAGE);
  builder.AddShaderArgument(argument);
  builder.AddShaderArgument(LightRenderComponent::GetDescriptorSetArgument());
  builder.AddShaderArgument(m_argument);

  builder.EndPipeline();
}

void
DirectLightPass::Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList) {
  auto inputSet = registry.GetInputDescriptorSet();
  auto framebuffer = registry.GetFrameBuffer();
  auto pipeline = registry.GetPipeline(0);
  const auto* scene = reinterpret_cast<Job::RenderUserData*>(registry.GetUserData())->scene;

  auto& world = scene->GetWorld();
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto& camera = scene->GetEditorCamera();
  m_cameraInfo.cameraPos = camera->GetPosition();
  m_cameraInfo.cameraView = camera->GetViewMatrix();
  bufCtx->UpdateBuffer(m_cameraInfoBuffer, &m_cameraInfo, sizeof(CameraInfo), 0);

  std::array<ViewportInfo, 1> viewport;
  viewport[0].x = 0;
  viewport[0].y = 0;
  viewport[0].width = m_width;
  viewport[0].height = m_height;
  viewport[0].minDepth = 0;
  viewport[0].maxDepth = 1;

  std::array<ScissorInfo, 1> scissor;
  scissor[0].x = 0;
  scissor[0].y = 0;
  scissor[0].height = m_height;
  scissor[0].width = m_width;

  auto renderLightDataView = world.view<LightRenderComponent>();
  DLOG_IF(WARNING, renderLightDataView.size() > 1)
      << FORMAT("multi {} in the scene", NAMEOF_TYPE(LightRenderComponent));
  auto& renderLightData = world.get<LightRenderComponent>(renderLightDataView[0]);
  auto& lightDataSet = renderLightData.m_lightSet;

  commandList.Begin();
  commandList.BeginPipeline(pipeline, framebuffer, {{0, 0, 0, 0}});
  commandList.SetViewports(viewport);
  commandList.SetScissors(scissor);
  commandList.BindDescriptorSet(pipeline, {inputSet, lightDataSet, m_descriptorSet});
  commandList.Draw(6, 1, 0, 0);
  commandList.EndPipeline(pipeline);
  commandList.End();
}

bool
DirectLightPass::IsEnable(RenderGraphGraphicsRegistry& registry) {
  const auto* scene = reinterpret_cast<Job::RenderUserData*>(registry.GetUserData())->scene;
  return scene != nullptr;
}

}  // namespace Marbas
