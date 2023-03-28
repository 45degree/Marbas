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
  builder.ReadTexture(m_transmittanceLUT);
  builder.SetFramebufferSize(m_width, m_height, 1);

  // pipeline create info
  RenderGraphPipelineCreateInfo createInfo;
  createInfo.SetPipelineLayout({
      DescriptorSetLayoutBinding{.bindingPoint = 0, .descriptorType = DescriptorType::UNIFORM_BUFFER},
      DescriptorSetLayoutBinding{.bindingPoint = 0, .descriptorType = DescriptorType::IMAGE},
  });
  createInfo.AddShader(ShaderType::VERTEX_SHADER, "Shader/transmittanceLUT.vert.spv");
  createInfo.AddShader(ShaderType::FRAGMENT_SHADER, "Shader/multiScatterLUT.frag.spv");
  createInfo.SetColorAttachmentsDesc({{
      .initAction = AttachmentInitAction::CLEAR,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::COLOR_RENDER_TARGET | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
      .format = ImageFormat::RGBA32F,
  }});
  createInfo.SetBlendConstance(0, 0, 0, 1);
  createInfo.SetBlendAttachments({BlendAttachment{.blendEnable = false}});
  DepthStencilCreateInfo depthStencil;
  depthStencil.depthTestEnable = false;
  createInfo.SetDepthStencil(depthStencil);

  builder.SetPipelineInfo(createInfo);
}

void
MultiScatterLUT::Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList) {
  auto* transmittanceLUT = registry.GetRenderBackendTextureSubResource(m_transmittanceLUT, 0, 1, 0, 1);

  commandList.SetDescriptorSetCount(1);

  RenderArgument argument;
  argument.BindUniformBuffer(0, m_atmosphereInfoBuffer);
  argument.BindImage(0, m_sampler, transmittanceLUT);

  commandList.Begin({{0, 0, 0, 0}});
  commandList.BindArgument(argument);
  commandList.Draw(6, 1, 0, 0);
  commandList.End();
}

}  // namespace Marbas
