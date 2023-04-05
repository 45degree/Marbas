#include "GridPass.hpp"

#include <nameof.hpp>

namespace Marbas {

GridRenderPass::GridRenderPass(const GridRenderPassCreateInfo& createInfo)
    : m_width(createInfo.width),
      m_height(createInfo.height),
      m_rhiFactory(createInfo.rhiFactory),
      m_scene(createInfo.scene),
      m_finalColorTexture(createInfo.finalColorTexture),
      m_finalDepthTexture(createInfo.finalDepthTexture) {
  auto* bufCtx = m_rhiFactory->GetBufferContext();
  auto* pipelineCtx = m_rhiFactory->GetPipelineContext();
  m_cameraInfoUBO = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_cameraInfo, sizeof(CameraInfo), false);

  m_argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
  m_descriptorSet = pipelineCtx->CreateDescriptorSet(m_argument);

  BindBufferInfo bindBufferInfo{
      .descriptorSet = m_descriptorSet,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
      .buffer = m_cameraInfoUBO,
      .offset = 0,
      .arrayElement = 0,
  };
  pipelineCtx->BindBuffer(bindBufferInfo);
}

GridRenderPass::~GridRenderPass() {
  auto* bufCtx = m_rhiFactory->GetBufferContext();
  bufCtx->DestroyBuffer(m_cameraInfoUBO);
}

void
GridRenderPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.WriteTexture(m_finalColorTexture);
  builder.WriteTexture(m_finalDepthTexture, TextureAttachmentType::DEPTH);

  builder.BeginPipeline();
  builder.AddShaderArgument(m_argument);
  builder.SetBlendConstant(0, 0, 0, 0);
  builder.AddBlendAttachments(BlendAttachment{
      .blendEnable = true,
      .srcColorBlendFactor = BlendFactor::SRC_ALPHA,
      .dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
      .srcAlphaBlendFactor = BlendFactor::SRC_ALPHA,
      .dstAlphaBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
  });
  builder.AddColorTarget(ColorTargetDesc{
      .initAction = AttachmentInitAction::KEEP,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::COLOR_RENDER_TARGET | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
      .format = ImageFormat::RGBA,
  });
  builder.SetDepthTarget(DepthTargetDesc{
      .initAction = AttachmentInitAction::KEEP,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::DEPTH_STENCIL,
      .sampleCount = SampleCount::BIT1,
  });
  builder.AddShader("Shader/grid.vert.spv", ShaderType::VERTEX_SHADER);
  builder.AddShader("Shader/grid.frag.spv", ShaderType::FRAGMENT_SHADER);
  builder.EndPipeline();

  builder.SetFramebufferSize(m_width, m_height, 1);
}

void
GridRenderPass::Execute(RenderGraphRegistry& registry, GraphicsCommandBuffer& commandList) {
  auto pipeline = registry.GetPipeline(0);
  auto framebuffer = registry.GetFrameBuffer();

  // set camera info
  auto camera = m_scene->GetEditorCamera();
  auto* bufCtx = m_rhiFactory->GetBufferContext();
  m_cameraInfo.up = camera->GetUpVector();
  m_cameraInfo.FAR = camera->GetFar();
  m_cameraInfo.NEAR = camera->GetNear();
  m_cameraInfo.pos = camera->GetPosition();
  m_cameraInfo.view = camera->GetViewMatrix();
  m_cameraInfo.right = camera->GetRightVector();
  m_cameraInfo.perspective = camera->GetProjectionMatrix();
  bufCtx->UpdateBuffer(m_cameraInfoUBO, &m_cameraInfo, sizeof(CameraInfo), 0);

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
  commandList.BindDescriptorSet(pipeline, {m_descriptorSet});
  commandList.Draw(6, 1, 0, 0);
  commandList.EndPipeline(pipeline);
  commandList.End();
}

}  // namespace Marbas
