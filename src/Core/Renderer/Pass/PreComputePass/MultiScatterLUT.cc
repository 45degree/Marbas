#include "MultiScatterLUT.hpp"

namespace Marbas {

MultiScatterLUT::MultiScatterLUT(const MultiScatterLUTCreateInfo& createInfo)
    : m_rhiFactory(createInfo.rhiFactory),
      m_transmittanceLUT(createInfo.transmittanceLUT),
      m_multiScatterLUT(createInfo.multiScatterLUT),
      m_width(createInfo.width),
      m_height(createInfo.height) {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  constexpr size_t size = sizeof(AtmosphereInfo);
  m_atmosphereInfoBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_atmosphereInfo, size, true);
  bufCtx->UpdateBuffer(m_atmosphereInfoBuffer, &m_atmosphereInfo, sizeof(AtmosphereInfo), 0);

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

  // set descirptor set
  m_argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
  m_transmittanceLUTArgument.Bind(0, DescriptorType::IMAGE);
  m_descriptorSet = pipelineCtx->CreateDescriptorSet(m_argument);
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_descriptorSet,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
      .buffer = m_atmosphereInfoBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
}

MultiScatterLUT::~MultiScatterLUT() {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  bufCtx->DestroyBuffer(m_atmosphereInfoBuffer);
  pipelineCtx->DestroySampler(m_sampler);
}

void
MultiScatterLUT::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.WriteTexture(m_multiScatterLUT);
  builder.ReadTexture(m_transmittanceLUT, m_sampler);
  builder.SetFramebufferSize(m_width, m_height, 1);

  // pipeline create info
  builder.BeginPipeline();
  builder.AddShaderArgument(m_argument);
  builder.AddShaderArgument(m_transmittanceLUTArgument);
  builder.AddShader("Shader/transmittanceLUT.vert.spv", ShaderType::VERTEX_SHADER);
  builder.AddShader("Shader/multiScatterLUT.frag.spv", ShaderType::FRAGMENT_SHADER);
  builder.AddColorTarget({
      .initAction = AttachmentInitAction::CLEAR,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::COLOR_RENDER_TARGET | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
      .format = ImageFormat::RGBA32F,
  });
  builder.SetBlendConstant(0, 0, 0, 1);
  builder.AddBlendAttachments({.blendEnable = false});
  builder.EnableDepthTest(false);
  builder.EndPipeline();
}

void
MultiScatterLUT::Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList) {
  auto pipeline = registry.GetPipeline(0);
  auto framebuffer = registry.GetFrameBuffer();
  auto inputSet = registry.GetInputDescriptorSet();

  /**
   * record command
   */
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
  commandList.BindDescriptorSet(pipeline, {m_descriptorSet, inputSet});
  commandList.Draw(6, 1, 0, 0);
  commandList.EndPipeline(pipeline);
  commandList.End();
}

}  // namespace Marbas
