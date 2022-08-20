#include "Core/Renderer/GridRenderPass.hpp"

#include <nameof.hpp>

#include "Core/Renderer/BlinnPhongRenderPass.hpp"
#include "Core/Renderer/GeometryRenderPass.hpp"
#include "RHI/Interface/Pipeline.hpp"

namespace Marbas {

GridRenderPassCreateInfo::GridRenderPassCreateInfo() {
  passName = "GridRenderPassCreateInfo";
  inputPassNode = BlinnPhongRenderPass::renderPassName;
}

GridRenderPass::GridRenderPass(const GridRenderPassCreateInfo& createInfo)
    : ForwardRenderPass(createInfo) {
  DLOG_ASSERT(m_rhiFactory != nullptr)
      << FORMAT("can't Initialize the {}, because the rhiFactory isn't been set",
                NAMEOF_TYPE(GridRenderPass));

  /**
   * set render pass and pipeline
   */

  // create render pass
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

  // create command factory
  m_commandFactory = m_rhiFactory->CreateCommandFactory();
  m_commandBuffer = m_commandFactory->CreateCommandBuffer();

  // read shader
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto shaderResource = shaderContainer->CreateResource();
  shaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/grid.vert.glsl");
  shaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/grid.frag.glsl");
  shaderResource->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shaderResource);

  // create Pipeline
  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = m_width, .height = m_height});
  m_pipeline->SetShader(shaderResource->GetShader());
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

  // create uniform buffer

  m_vertexBuffer = m_rhiFactory->CreateVertexBuffer(0);
  auto bufferSize = sizeof(GridRenderPass::MatrixUniformBufferBlock);
  m_uniformBuffer = m_rhiFactory->CreateUniformBuffer(bufferSize);

  DescriptorSetInfo descriptorSetInfo{
      DescriptorInfo{
          .isBuffer = true,
          .type = BufferDescriptorType::UNIFORM_BUFFER,
          .bindingPoint = 0,
      },
  };
  m_descriptorSet = m_rhiFactory->CreateDescriptorSet(descriptorSetInfo);
  m_descriptorSet->BindBuffer(0, m_uniformBuffer->GetIBufferDescriptor());
}

void
GridRenderPass::RecordCommand(const Scene* scene) {
  m_commandBuffer->Clear();

  // check framebuffer and renderpass
  DLOG_ASSERT(m_framebuffer != nullptr)
      << FORMAT("{}'s framebuffer is null, can't record command", NAMEOF_TYPE(GridRenderPass));

  DLOG_ASSERT(m_renderPass != nullptr)
      << FORMAT("{}'s render pass is null, can't record command", NAMEOF_TYPE(GridRenderPass));

  DLOG_ASSERT(m_pipeline != nullptr)
      << FORMAT("{}'s pipeline is null, can't record command", NAMEOF_TYPE(GridRenderPass));

  // recreate uniform buffer

  auto bufferSize = sizeof(GridRenderPass::MatrixUniformBufferBlock);
  m_uniformBuffer->SetData(&m_matrixUniformBlock, bufferSize, 0);

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
  const auto viewMatrix = editorCamera->GetViewMartix();
  const auto perspectiveMatrix = editorCamera->GetPerspective();

  m_matrixUniformBlock.view = viewMatrix;
  m_matrixUniformBlock.perspective = perspectiveMatrix;
  m_uniformBuffer->SetData(&m_matrixUniformBlock, sizeof(MatrixUniformBufferBlock), 0);
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
