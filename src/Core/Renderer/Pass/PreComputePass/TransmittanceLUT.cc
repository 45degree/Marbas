#include "TransmittanceLUT.hpp"

namespace Marbas {

TransmittanceLUTPass::TransmittanceLUTPass(const TransmittanceLUTPassCreateInfo& createInfo)
    : m_rhiFactory(createInfo.rhiFactory),
      m_width(createInfo.width),
      m_height(createInfo.height),
      m_lutTexture(createInfo.lutTexture) {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  constexpr size_t size = sizeof(AtmosphereInfo);
  m_atmosphereInfoBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_atmosphereInfo, size, true);
  bufCtx->UpdateBuffer(m_atmosphereInfoBuffer, &m_atmosphereInfo, sizeof(AtmosphereInfo), 0);
}

TransmittanceLUTPass::~TransmittanceLUTPass() {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  bufCtx->DestroyBuffer(m_atmosphereInfoBuffer);
}

void
TransmittanceLUTPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.SetFramebufferSize(m_width, m_height, 1);
  builder.WriteTexture(m_lutTexture);

  // pipeline create info
  RenderGraphPipelineCreateInfo createInfo;
  createInfo.SetPipelineLayout({
      DescriptorSetLayoutBinding{.bindingPoint = 0, .descriptorType = DescriptorType::UNIFORM_BUFFER},
  });
  createInfo.AddShader(ShaderType::VERTEX_SHADER, "Shader/transmittanceLUT.vert.spv");
  createInfo.AddShader(ShaderType::FRAGMENT_SHADER, "Shader/transmittanceLUT.frag.spv");
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
TransmittanceLUTPass::Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList) {
  commandList.SetDescriptorSetCount(1);

  RenderArgument argument;
  argument.BindUniformBuffer(0, m_atmosphereInfoBuffer);

  commandList.Begin({{0, 0, 0, 0}});
  commandList.BindArgument(argument);
  commandList.Draw(6, 1, 0, 0);
  commandList.End();
}

}  // namespace Marbas
