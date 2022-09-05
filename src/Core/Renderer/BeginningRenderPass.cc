#include "Core/Renderer/BeginningRenderPass.hpp"

#include <nameof.hpp>

namespace Marbas {

const String BeginningRenderPass::renderPassName = "beginningRenderPass";
const String BeginningRenderPass::targetName = "beginningTarget";
const String BeginningRenderPass::depthTargetName = "beginningDepthTarget";

BeginningRenderPassCreateInfo::BeginningRenderPassCreateInfo() : DeferredRenderPassCreateInfo() {
  passName = BeginningRenderPass::renderPassName;
  inputResource = {};
  outputResource = {BeginningRenderPass::depthTargetName, BeginningRenderPass::targetName};
}

BeginningRenderPass::BeginningRenderPass(const BeginningRenderPassCreateInfo& createInfo)
    : DeferredRenderPass(createInfo) {}

void
BeginningRenderPass::CreateRenderPass() {
  RenderPassCreateInfo renderPassCreateInfo{
      .attachments =
          {
              AttachmentDescription{
                  .format = TextureFormat::RGBA,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Clear,
              },
              AttachmentDescription{
                  .format = TextureFormat::DEPTH,
                  .type = AttachmentType::Depth,
                  .loadOp = AttachmentLoadOp::Clear,
              },
          },
  };
  m_renderPass = m_rhiFactory->CreateRenderPass(renderPassCreateInfo);
}

void
BeginningRenderPass::CreatePipeline() {
  // create pipeline
  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = m_width, .height = m_height});
}

void
BeginningRenderPass::CreateFrameBuffer() {
  const auto& targetGBuffer = m_outputTarget[targetName]->GetGBuffer();
  auto colorBuffer = targetGBuffer->GetTexture(GBufferTexutreType::COLOR);

  const auto& depthGBuffer = m_outputTarget[depthTargetName]->GetGBuffer();
  auto depthBuffer = depthGBuffer->GetTexture(GBufferTexutreType::DEPTH);

  if (colorBuffer == nullptr) {
    LOG(ERROR) << "can't get normal buffer or color buffer from the gbuffer";
    throw std::runtime_error("normal buffer and color buffer is needed by geometry render pass");
  }

  auto width = colorBuffer->GetWidth();
  auto height = colorBuffer->GetHeight();

  FrameBufferInfo createInfo{
      .width = width,
      .height = height,
      .renderPass = m_renderPass.get(),
      .attachments = {colorBuffer, depthBuffer},
  };

  m_framebuffer = m_rhiFactory->CreateFrameBuffer(createInfo);
}

void
BeginningRenderPass::RecordCommand(const Scene* scene) {
  m_commandBuffer->Clear();

  // check framebuffer and renderpass
  DLOG_ASSERT(m_framebuffer != nullptr);
  DLOG_ASSERT(m_renderPass != nullptr);
  DLOG_ASSERT(m_pipeline != nullptr);

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
  m_commandBuffer->AddCommand(std::move(endRenderPass));
  m_commandBuffer->EndRecordCmd();
}

void
BeginningRenderPass::Execute(const Scene* scene, const ResourceManager* resourceManager) {
  if (m_needToRecordComand) {
    RecordCommand(scene);
    m_needToRecordComand = false;
  }
  m_commandBuffer->SubmitCommand();
}

}  // namespace Marbas
