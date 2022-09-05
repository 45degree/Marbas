#include "Core/Renderer/GridRenderPass.hpp"

#include <nameof.hpp>

#include "Core/Renderer/BlinnPhongRenderPass.hpp"
#include "Core/Renderer/GeometryRenderPass.hpp"
#include "Core/Renderer/PointLightShadowMappingRenderPass.hpp"
#include "RHI/Interface/Pipeline.hpp"
#include "ShadowMappingRenderPass.hpp"

namespace Marbas {

GridRenderPassCreateInfo::GridRenderPassCreateInfo() {
  passName = "GridRenderPassCreateInfo";
  inputPassNode = PointLightShadowMappingRenderPass::renderPassName;
}

GridRenderPass::GridRenderPass(const GridRenderPassCreateInfo& createInfo)
    : ForwardRenderPass(createInfo) {}

void
GridRenderPass::OnInit() {
  m_descriptorSet = m_rhiFactory->CreateDescriptorSet(m_descriptorSetLayout);
  m_descriptorSet->BindBuffer(0, m_cameraUniformBuffer);
  m_vertexBuffer = m_rhiFactory->CreateVertexBuffer(0);
}

void
GridRenderPass::CreateRenderPass() {
  RenderPassCreateInfo renderPassCreateInfo{
      .attachments =
          {
              AttachmentDescription{
                  // result attachment
                  .format = TextureFormat::RGBA,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Ignore,
              },
              AttachmentDescription{
                  // input depth attachment from geometryRenderPass's output
                  .format = TextureFormat::DEPTH,
                  .type = AttachmentType::Depth,
                  .loadOp = AttachmentLoadOp::Ignore,
              },
          },
  };
  m_renderPass = m_rhiFactory->CreateRenderPass(renderPassCreateInfo);
}

void
GridRenderPass::CreateShader() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto shaderResource = shaderContainer->CreateResource();
  shaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/grid.vert.glsl");
  shaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/grid.frag.glsl");
  shaderResource->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shaderResource);
}

void
GridRenderPass::CreatePipeline() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto resource = shaderContainer->GetResource(m_shaderId);
  if (!resource->IsLoad()) {
    resource->LoadResource(m_rhiFactory, m_resourceManager.get());
  }

  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = m_width, .height = m_height});
  m_pipeline->SetShader(resource->GetShader());
  m_pipeline->SetVertexBufferLayout({}, VertexInputRate::INSTANCE);
  m_pipeline->SetVertexInputBindingDivisor({{0, 1}});
  m_pipeline->SetDepthStencilInfo(DepthStencilInfo{
      .depthTestEnable = true,
  });
  m_pipeline->SetBlendInfo(BlendInfo{
      .logicOpEnable = false,
      .logicOp = LogicOp::COPY,
      .attachments =
          {
              BlendAttachment{
                  .blendEnable = true,
                  .srcColorBlendFactor = BlendFactor::SRC_ALPHA,
                  .dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_COLOR,
                  .srcAlphaBlendFactor = BlendFactor::SRC_ALPHA,
                  .dstAlphaBlendFactor = BlendFactor::ONE_MINUS_SRC_COLOR,
              },
          },
      .constances = {0, 0, 0, 0},
  });

  m_pipeline->Create();
}

void
GridRenderPass::RecordCommand(const Scene* scene) {
  m_commandBuffer->Clear();

  // check framebuffer and renderpass
  DLOG_ASSERT(m_framebuffer != nullptr);
  DLOG_ASSERT(m_renderPass != nullptr);
  DLOG_ASSERT(m_pipeline != nullptr);

  // recreate uniform buffer

  /**
   * set command
   */

  // set render pass command
  auto beginRenderPass = m_commandFactory->CreateBeginRenderPassCMD();
  beginRenderPass->SetRenderPass(m_renderPass);
  beginRenderPass->SetFrameBuffer(m_framebuffer);
  beginRenderPass->SetClearColor({0, 0, 0, 1});

  auto endRenderPass = m_commandFactory->CreateEndRenderPassCMD();
  endRenderPass->SetRenderPass(m_renderPass);
  endRenderPass->SetFrameBuffer(m_framebuffer);

  // set bind pipeline
  auto bindPipeline = m_commandFactory->CreateBindPipelineCMD();
  bindPipeline->SetPipeLine(m_pipeline);

  /**
   * begin to record command
   */

  m_commandBuffer->BeginRecordCmd();
  m_commandBuffer->AddCommand(std::move(beginRenderPass));
  m_commandBuffer->AddCommand(std::move(bindPipeline));

  auto bindVertexBuffer = m_commandFactory->CreateBindVertexBufferCMD();
  auto bindDescriptorSet = m_commandFactory->CreateBindDescriptorSetCMD();
  auto drawArray = m_commandFactory->CreateDrawArrayCMD(m_pipeline);

  bindVertexBuffer->SetVertexBuffer(m_vertexBuffer);
  drawArray->SetVertexCount(6);
  drawArray->SetInstanceCount(1);
  bindDescriptorSet->SetDescriptor(m_descriptorSet);

  m_commandBuffer->AddCommand(std::move(bindDescriptorSet));
  m_commandBuffer->AddCommand(std::move(drawArray));

  m_commandBuffer->AddCommand(std::move(endRenderPass));
  m_commandBuffer->EndRecordCmd();
}

void
GridRenderPass::SetUniformBuffer(const Scene* scene) {
  const auto editorCamera = scene->GetEditorCamrea();
  UpdateCameraUniformBuffer(editorCamera.get());
}

void
GridRenderPass::Execute(const Scene* scene, const ResourceManager* resourceManager) {
  if (m_needToRecordComand) {
    RecordCommand(scene);
    m_needToRecordComand = false;
  }

  SetUniformBuffer(scene);
  m_commandBuffer->SubmitCommand();
}

}  // namespace Marbas
