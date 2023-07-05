#include "Core/Scene/System/RenderSystemJob/RenderSystem.hpp"
#include "VoxelVisualizatonPass.hpp"

namespace Marbas::GI {

VoxelVisulzationPass::VoxelVisulzationPass(const VoxelVisulzationPassCreateInfo& createInfo)
    : m_rhiFactory(createInfo.rhiFactory),
      m_width(createInfo.m_width),
      m_height(createInfo.m_height),
      m_voxelTexture(createInfo.m_voxelTexture),
      m_resultTexture(createInfo.m_resultTexture),
      m_depthTexture(createInfo.m_depthTexture) {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  m_voxelInfoBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_voxelInfo, sizeof(m_voxelInfo), false);
  m_cameraInfoBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_cameraInfo, sizeof(m_cameraInfo), false);

  m_argument.Clear();
  m_argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
  m_argument.Bind(1, DescriptorType::UNIFORM_BUFFER);
  m_set = pipelineCtx->CreateDescriptorSet(m_argument);
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

void
VoxelVisulzationPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.ReadTexture(m_voxelTexture, m_sampler);
  builder.WriteTexture(m_resultTexture, TextureAttachmentType::COLOR);
  builder.WriteTexture(m_depthTexture, TextureAttachmentType::DEPTH);

  DescriptorSetArgument inputArgument;
  inputArgument.Bind(0, DescriptorType::IMAGE);

  builder.BeginPipeline();
  builder.AddShaderArgument(inputArgument);
  builder.AddShaderArgument(m_argument);
  builder.AddShader("Shader/voxelVisualization.vert.spv", ShaderType::VERTEX_SHADER);
  builder.AddShader("Shader/voxelVisualization.geom.spv", ShaderType::GEOMETRY_SHADER);
  builder.AddShader("Shader/voxelVisualization.frag.spv", ShaderType::FRAGMENT_SHADER);
  builder.AddColorTarget({
      .initAction = AttachmentInitAction::CLEAR,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::COLOR_RENDER_TARGET | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
      .format = ImageFormat::RGBA32F,
  });
  builder.SetDepthTarget({
      .initAction = AttachmentInitAction::CLEAR,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::DEPTH_STENCIL | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
  });
  builder.EnableDepthTest(true);
  builder.AddBlendAttachments({false});
  builder.SetBlendConstant(0, 0, 0, 0);
  builder.SetPrimitiveType(PrimitiveTopology::POINT);
  builder.EndPipeline();

  builder.SetFramebufferSize(m_width, m_height, 1);
}

void
VoxelVisulzationPass::Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList) {
  const auto* scene = reinterpret_cast<Job::RenderUserData*>(registry.GetUserData())->scene;

  auto& world = scene->GetWorld();
  auto camera = scene->GetEditorCamera();
  auto* bufferContext = m_rhiFactory->GetBufferContext();
  auto* pipelineContext = m_rhiFactory->GetPipelineContext();

  m_cameraInfo.view = camera->GetViewMatrix();
  m_cameraInfo.projection = camera->GetProjectionMatrix();
  bufferContext->UpdateBuffer(m_cameraInfoBuffer, &m_cameraInfo, sizeof(CameraInfo), 0);

  /**
   * record command
   */
  auto pipeline = registry.GetPipeline(0);
  auto framebuffer = registry.GetFrameBuffer();
  auto voxelSet = registry.GetInputDescriptorSet();

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
  commandList.BeginPipeline(pipeline, framebuffer, {{0, 0, 0, 0}, {1, 1}});
  commandList.SetViewports(viewport);
  commandList.SetScissors(scissor);
  commandList.BindDescriptorSet(pipeline, {voxelSet, m_set});
  commandList.Draw(256 * 256 * 256, 1, 0, 0);
  commandList.EndPipeline(pipeline);
  commandList.End();
}

bool
VoxelVisulzationPass::IsEnable(RenderGraphGraphicsRegistry& registry) {
  return true;
}

}  // namespace Marbas::GI
