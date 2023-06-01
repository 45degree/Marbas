#include "VXGIPass.hpp"

namespace Marbas::GI {

VXGIPass::VXGIPass(const VXGIPassCreateInfo& createInfo)
    : m_rhiFactory(createInfo.m_rhiFactory),
      m_width(createInfo.m_widht),
      m_height(createInfo.m_height),
      m_voxelTexture(createInfo.m_voxelTexture),
      m_positionRoughnessTexture(createInfo.m_positionRoughnessTexture),
      m_normalMetallicTexture(createInfo.m_normalMetallicTexture),
      m_diffuseTexture(createInfo.m_diffuseTexture),
      m_finalTexture(createInfo.m_finalTexture) {
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
      .maxLod = 8,
      .borderColor = Marbas::BorderColor::IntOpaqueBlack,
  };
  m_sampler = pipelineCtx->CreateSampler(samplerCreateInfo);

  m_argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
  m_argument.Bind(1, DescriptorType::UNIFORM_BUFFER);
  m_set = pipelineCtx->CreateDescriptorSet(m_argument);

  m_cameraInfoBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_cameraInfo, sizeof(CameraInfo), false);
  m_voxelInfoBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_voxelInfo, sizeof(VoxelInfo), false);

  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_set,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
      .buffer = m_voxelInfoBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_set,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 1,
      .buffer = m_cameraInfoBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
}

void
VXGIPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.ReadTexture(m_voxelTexture, m_sampler, 0, 1, 0, 8);
  builder.ReadTexture(m_positionRoughnessTexture, m_sampler);
  builder.ReadTexture(m_normalMetallicTexture, m_sampler);
  builder.ReadTexture(m_diffuseTexture, m_sampler);
  builder.WriteTexture(m_finalTexture, TextureAttachmentType::COLOR);

  DescriptorSetArgument argument;
  argument.Bind(0, DescriptorType::IMAGE);
  argument.Bind(1, DescriptorType::IMAGE);
  argument.Bind(2, DescriptorType::IMAGE);
  argument.Bind(3, DescriptorType::IMAGE);

  builder.BeginPipeline();
  builder.AddShaderArgument(argument);
  builder.AddShaderArgument(m_argument);
  builder.EnableDepthTest(false);
  builder.AddColorTarget(ColorTargetDesc{
      .initAction = AttachmentInitAction::CLEAR,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
      .sampleCount = SampleCount::BIT1,
      .format = ImageFormat::RGBA32F,
  });
  builder.AddBlendAttachments(BlendAttachment{.blendEnable = false});
  builder.AddShader("Shader/ScreenSpace.vert.spv", ShaderType::VERTEX_SHADER);
  builder.AddShader("Shader/vxgi.frag.spv", ShaderType::FRAGMENT_SHADER);
  builder.EndPipeline();

  builder.SetFramebufferSize(m_width, m_height, 1);
}

void
VXGIPass::Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList) {
  auto inputSet = registry.GetInputDescriptorSet();
  auto pipeline = registry.GetPipeline(0);
  auto framebuffer = registry.GetFrameBuffer();
  auto scene = registry.GetCurrentActiveScene();

  auto voxelImage = registry.GetImage(m_voxelTexture);

  m_cameraInfo.cameraPos = scene->GetEditorCamera()->GetPosition();
  auto bufferCtx = m_rhiFactory->GetBufferContext();
  bufferCtx->UpdateBuffer(m_cameraInfoBuffer, &m_cameraInfo, sizeof(CameraInfo), 0);

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
  commandList.GenerateMipmap(voxelImage, voxelImage->mipMapLevel);
  commandList.BeginPipeline(pipeline, framebuffer, {{0, 0, 0, 0}});
  commandList.SetViewports(viewport);
  commandList.SetScissors(scissor);
  commandList.BindDescriptorSet(pipeline, {inputSet, m_set});
  commandList.Draw(6, 1, 0, 0);
  commandList.EndPipeline(pipeline);
  commandList.End();
}

}  // namespace Marbas::GI
