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
  m_cameraInfoUBO = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_cameraInfo, sizeof(CameraInfo), false);
}

GridRenderPass::~GridRenderPass() {
  auto* bufCtx = m_rhiFactory->GetBufferContext();
  bufCtx->DestroyBuffer(m_cameraInfoUBO);
}

void
GridRenderPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.WriteTexture(m_finalColorTexture);
  builder.WriteTexture(m_finalDepthTexture, TextureAttachmentType::DEPTH);

  RenderGraphPipelineCreateInfo createInfo;
  createInfo.SetPipelineLayout({
      DescriptorSetLayoutBinding{.bindingPoint = 0, .descriptorType = DescriptorType::UNIFORM_BUFFER},
  });
  createInfo.SetBlendConstance(0, 0, 0, 0);
  createInfo.SetBlendAttachments({BlendAttachment{
      .blendEnable = true,
      .srcColorBlendFactor = BlendFactor::SRC_ALPHA,
      .dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
      .srcAlphaBlendFactor = BlendFactor::SRC_ALPHA,
      .dstAlphaBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
  }});
  createInfo.SetColorAttachmentsDesc({{
      .initAction = AttachmentInitAction::KEEP,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::COLOR_RENDER_TARGET | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
      .format = ImageFormat::RGBA,
  }});
  createInfo.SetDepthAttachmentDesc({
      .initAction = AttachmentInitAction::KEEP,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::DEPTH_STENCIL,
      .sampleCount = SampleCount::BIT1,
  });
  createInfo.AddShader(ShaderType::VERTEX_SHADER, "Shader/grid.vert.spv");
  createInfo.AddShader(ShaderType::FRAGMENT_SHADER, "Shader/grid.frag.spv");
  builder.SetPipelineInfo(createInfo);

  builder.SetFramebufferSize(m_width, m_height, 1);
}

void
GridRenderPass::Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList) {
  // check framebuffer and renderpass
  commandList.SetDescriptorSetCount(1);

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

  RenderArgument argument;
  argument.BindUniformBuffer(0, m_cameraInfoUBO);

  /**
   * set command
   */
  commandList.Begin({{0, 0, 0, 0}, {1, 1}});
  commandList.BindArgument(argument);
  commandList.Draw(6, 1, 0, 0);
  commandList.End();
}

}  // namespace Marbas
