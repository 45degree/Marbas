#include "SSAOPass.hpp"

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"

namespace Marbas {

SSAOPass::SSAOPass(const SSAOCreateInfo& createInfo)
    : m_width(createInfo.width),
      m_height(createInfo.height),
      m_posTexture(createInfo.posTexture),
      m_normalTexture(createInfo.normalTexture),
      m_depthTexture(createInfo.depthTexture),
      m_ssaoTexture(createInfo.ssaoTexture),
      m_rhiFactory(createInfo.rhiFactory),
      m_scene(createInfo.scene) {
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

  m_cameraBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_cameraInfo, sizeof(CameraInfo), false);
  m_argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
  m_set = pipelineCtx->CreateDescriptorSet(m_argument);
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_set,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
      .buffer = m_cameraBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
}

void
SSAOPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.ReadTexture(m_posTexture, m_sampler);
  builder.ReadTexture(m_normalTexture, m_sampler);
  builder.ReadTexture(m_depthTexture, m_sampler);
  builder.WriteTexture(m_ssaoTexture, TextureAttachmentType::COLOR);

  builder.SetFramebufferSize(m_width, m_height, 1);

  builder.BeginPipeline();
  builder.EnableDepthTest(false);
  builder.AddColorTarget(ColorTargetDesc{
      .initAction = AttachmentInitAction::CLEAR,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
      .sampleCount = SampleCount::BIT1,
      .format = ImageFormat::R32F,
  });
  builder.AddBlendAttachments(BlendAttachment{.blendEnable = false});
  builder.AddShader("Shader/ScreenSpace.vert.spv", ShaderType::VERTEX_SHADER);
  builder.AddShader("Shader/SSAO.frag.spv", ShaderType::FRAGMENT_SHADER);

  DescriptorSetArgument argument;
  argument.Bind(0, DescriptorType::IMAGE);
  argument.Bind(1, DescriptorType::IMAGE);
  argument.Bind(2, DescriptorType::IMAGE);
  builder.AddShaderArgument(argument);
  builder.AddShaderArgument(m_argument);

  builder.EndPipeline();
}

void
SSAOPass::Execute(RenderGraphRegistry& registry, GraphicsCommandBuffer& commandList) {
  auto inputSet = registry.GetInputDescriptorSet();
  auto pipeline = registry.GetPipeline(0);
  auto framebuffer = registry.GetFrameBuffer();

  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto camera = m_scene->GetEditorCamrea();
  m_cameraInfo.m_far = camera->GetFar();
  m_cameraInfo.m_near = camera->GetNear();
  m_cameraInfo.m_position = camera->GetPosition();
  m_cameraInfo.m_viewMatrix = camera->GetViewMatrix();
  m_cameraInfo.m_projectMatrix = camera->GetProjectionMatrix();
  bufCtx->UpdateBuffer(m_cameraBuffer, &m_cameraInfo, sizeof(CameraInfo), 0);

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

  commandList.Begin();
  commandList.BeginPipeline(pipeline, framebuffer, {{0, 0, 0, 0}});
  commandList.SetViewports(viewport);
  commandList.SetScissors(scissor);
  commandList.BindDescriptorSet(pipeline, {inputSet, m_set});
  commandList.Draw(6, 1, 0, 0);
  commandList.EndPipeline(pipeline);
  commandList.End();
}

}  // namespace Marbas
