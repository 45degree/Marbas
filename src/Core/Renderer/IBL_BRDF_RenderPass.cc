#include "Core/Renderer/IBL_BRDF_RenderPass.hpp"

namespace Marbas {

IBLBRDFRenderPassCreateInfo::IBLBRDFRenderPassCreateInfo() : DeferredRenderPassCreateInfo() {
  passName = String(IBLBRDFRenderPass::renderPassName);
  inputResource = {};
  outputResource = {String(IBLBRDFRenderPass::targetName)};
}

IBLBRDFRenderPass::IBLBRDFRenderPass(const IBLBRDFRenderPassCreateInfo& createInfo)
    : DeferredRenderPass(createInfo) {}

void
IBLBRDFRenderPass::OnInit() {
  m_vertexBuffer = m_rhiFactory->CreateVertexBuffer(0);
}

void
IBLBRDFRenderPass::CreateRenderPass() {
  RenderPassCreateInfo renderPassCreateInfo{
      .attachments =
          {
              AttachmentDescription{
                  .format = TextureFormat::RG16F,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Clear,
              },
          },
  };

  m_renderPass = m_rhiFactory->CreateRenderPass(renderPassCreateInfo);
}

void
IBLBRDFRenderPass::CreateDescriptorSetLayout(){};

void
IBLBRDFRenderPass::CreateFrameBuffer() {
  const auto& targetGBuffer = m_outputTarget[String(targetName)];
  auto targetBuffer = targetGBuffer->GetGBuffer(GBufferTexutreType::IBL_BRDF_LOD);

  std::shared_ptr targetBufferView = m_rhiFactory->CreateImageView(ImageViewDesc{
      .m_texture = targetBuffer,
      .m_format = targetBuffer->GetFormat(),
      .m_type = targetBuffer->GetTextureType(),
  });
  FrameBufferInfo createInfo{
      .width = 512,
      .height = 512,
      .renderPass = m_renderPass.get(),
      .attachments = {targetBufferView},
  };

  m_framebuffer = m_rhiFactory->CreateFrameBuffer(createInfo);
}

void
IBLBRDFRenderPass::CreateShader() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto shaderResource = shaderContainer->CreateResource();
  shaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/IBLBRDF.vert.glsl");
  shaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/IBLBRDF.frag.glsl");
  shaderResource->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shaderResource);
}

void
IBLBRDFRenderPass::CreatePipeline() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto resource = shaderContainer->GetResource(m_shaderId);
  if (!resource->IsLoad()) {
    resource->LoadResource(m_rhiFactory, m_resourceManager.get());
  }

  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = 512, .height = 512});
  m_pipeline->SetVertexBufferLayout({}, VertexInputRate::INSTANCE);
  m_pipeline->SetPipelineLayout(GraphicsPipeLineLayout{
      .descriptorSetLayout = m_descriptorSetLayout,
  });
  m_pipeline->SetShader(resource->GetShader());
  m_pipeline->Create();
}

void
IBLBRDFRenderPass::RecordCommand(const Scene* scene) {
  m_commandBuffer->Clear();

  // check framebuffer and renderpass
  DLOG_ASSERT(m_framebuffer != nullptr);
  DLOG_ASSERT(m_renderPass != nullptr);
  DLOG_ASSERT(m_pipeline != nullptr);

  m_commandBuffer->BeginRecordCmd();
  m_commandBuffer->BeginRenderPass(BeginRenderPassInfo{
      .renderPass = m_renderPass,
      .frameBuffer = m_framebuffer,
      .clearColor = {0, 0, 0, 1},
  });
  m_commandBuffer->BindPipeline(m_pipeline);
  m_commandBuffer->BindVertexBuffer(m_vertexBuffer);
  m_commandBuffer->DrawArray(6, 1);
  m_commandBuffer->EndRenderPass();
  m_commandBuffer->EndRecordCmd();
}

void
IBLBRDFRenderPass::Execute(const Scene* scene, const ResourceManager* resourceMnager) {
  if (m_needToRecordComand) {
    RecordCommand(scene);
    m_needToRecordComand = false;
  }
  m_commandBuffer->SubmitCommand();
}

}  // namespace Marbas
