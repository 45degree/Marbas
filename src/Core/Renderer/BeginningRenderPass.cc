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

  std::shared_ptr colorBufferView = m_rhiFactory->CreateImageView();
  colorBufferView->SetTexture(colorBuffer);
  std::shared_ptr depthBufferView = m_rhiFactory->CreateImageView();
  depthBufferView->SetTexture(depthBuffer);

  FrameBufferInfo createInfo{
      .width = width,
      .height = height,
      .renderPass = m_renderPass.get(),
      .attachments = {colorBufferView, depthBufferView},
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

  m_commandBuffer->BeginRecordCmd();
  m_commandBuffer->BeginRenderPass(BeginRenderPassInfo{
      .renderPass = m_renderPass,
      .frameBuffer = m_framebuffer,
      .clearColor = {0, 0, 0, 1},
  });
  m_commandBuffer->EndRenderPass();
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
