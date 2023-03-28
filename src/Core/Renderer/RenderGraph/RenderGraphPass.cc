#include "RenderGraphPass.hpp"

#include <glog/logging.h>

#include "RenderGraph.hpp"

namespace Marbas::details {

RenderGraphPass::RenderGraphPass(StringView name, RHIFactory* rhiFactory)
    : RenderGraphNode(name, RenderGraphNodeType::Pass), m_rhiFactory(rhiFactory) {}

RenderGraphPass::~RenderGraphPass() {}

RenderGraphGraphicsPass::RenderGraphGraphicsPass(StringView name, RHIFactory* rhiFactory)
    : RenderGraphPass(name, rhiFactory) {}

RenderGraphGraphicsPass::~RenderGraphGraphicsPass() {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  if (m_framebuffer != nullptr) {
    pipelineCtx->DestroyFrameBuffer(m_framebuffer);
  }
  if (m_pipeline != nullptr) {
    pipelineCtx->DestroyPipeline(m_pipeline);
  }
}

void
RenderGraphGraphicsPass::AddSubResDesc(TextureAttachmentType type, const RenderGraphTextureHandler& handler,
                                       uint32_t baseLayer, uint32_t layerCount, uint32_t baseLevel,
                                       uint32_t levelCount) {
  switch (type) {
    case TextureAttachmentType::COLOR:
      m_colorAttachment.push_back({handler, baseLayer, layerCount, baseLevel, levelCount});
      break;
    case TextureAttachmentType::DEPTH:
      m_depthAttachment = {handler, baseLayer, layerCount, baseLevel, levelCount};
      break;
    case TextureAttachmentType::RESOLVE:
      m_resolveAttachment.push_back({handler, baseLayer, layerCount, baseLevel, levelCount});
      break;
  }
}

void
RenderGraphGraphicsPass::Initialize(RenderGraph* graph) {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto bufCtx = m_rhiFactory->GetBufferContext();

  m_commandList = std::make_unique<GraphicsRenderCommandList>(this, m_rhiFactory);

  const auto& bindings = m_pipelineCreateInfo.m_bindings;
  m_layout = pipelineCtx->CreateDescriptorSetLayout(bindings);

  /**
   * create pipeline
   */

  GraphicsPipeLineCreateInfo pipelineCreateInfo;
  pipelineCreateInfo.layout = m_layout;
  pipelineCreateInfo.shaderStageCreateInfo = m_pipelineCreateInfo.m_shaders;
  pipelineCreateInfo.multisampleCreateInfo.rasterizationSamples = m_pipelineCreateInfo.m_sampleCount;
  pipelineCreateInfo.outputRenderTarget.colorAttachments = m_pipelineCreateInfo.m_colorAttachments;
  pipelineCreateInfo.outputRenderTarget.depthAttachments = m_pipelineCreateInfo.m_depthAttachments;
  pipelineCreateInfo.outputRenderTarget.resolveAttachments = m_pipelineCreateInfo.m_resolveAttachments;
  pipelineCreateInfo.blendInfo.constances = m_pipelineCreateInfo.m_blendConstance;
  pipelineCreateInfo.blendInfo.attachments = m_pipelineCreateInfo.m_blendAttachment;
  pipelineCreateInfo.vertexInputLayout.elementDesc = m_pipelineCreateInfo.m_inputElementDesc;
  pipelineCreateInfo.vertexInputLayout.viewDesc = m_pipelineCreateInfo.m_inputElementView;
  pipelineCreateInfo.rasterizationInfo = m_pipelineCreateInfo.m_rasterizationCreateInfo;
  pipelineCreateInfo.depthStencilInfo = m_pipelineCreateInfo.m_depthStencilCreateInfo;
  m_pipeline = pipelineCtx->CreatePipeline(pipelineCreateInfo);

  /**
   * create framebuffer
   */
  auto GetImageViewFromDesc = [&graph](const SubResourceDesc& desc) -> ImageView* {
    if (desc.handler == std::nullopt) return nullptr;
    DLOG_ASSERT((*desc.handler).index < graph->m_resourceManager->m_graphTexture.size())
        << FORMAT("resource {} is not create", (*desc.handler).index);

    auto& res = graph->m_resourceManager->m_graphTexture[(*desc.handler).index];
    return res.GetImageView(desc.baseLayer, desc.layerCount, desc.baseLevel, desc.levelCount);
  };

  // get attachment image view
  Vector<ImageView*> colorAttachment;
  Vector<ImageView*> resolveAttachment;
  ImageView* depthAttachment = nullptr;

  for (auto& attachment : m_colorAttachment) {
    colorAttachment.push_back(GetImageViewFromDesc(attachment));
  }

  if (m_depthAttachment.handler != std::nullopt) {
    depthAttachment = GetImageViewFromDesc(m_depthAttachment);
  }

  for (auto& attachment : m_resolveAttachment) {
    resolveAttachment.push_back(GetImageViewFromDesc(attachment));
  }

  // create framebuffer
  FrameBufferCreateInfo framebufferCreateInfo;
  framebufferCreateInfo.layer = m_framebufferLayer;
  framebufferCreateInfo.height = m_framebufferHeight;
  framebufferCreateInfo.width = m_framebufferWidth;
  framebufferCreateInfo.attachments.colorAttachments = colorAttachment;
  framebufferCreateInfo.attachments.depthAttachment = depthAttachment;
  framebufferCreateInfo.attachments.resolveAttachments = resolveAttachment;
  framebufferCreateInfo.pieline = m_pipeline;
  m_framebuffer = pipelineCtx->CreateFrameBuffer(framebufferCreateInfo);
}

LambdaGraphicsRenderGraphPass::LambdaGraphicsRenderGraphPass(StringView name, RHIFactory* rhiFactory)
    : RenderGraphGraphicsPass(name, rhiFactory) {}

void
LambdaGraphicsRenderGraphPass::Execute(RenderGraph* graph) {
  RenderGraphRegistry registry(graph);
  m_commandList->BeginRecord();
  m_command(registry, *m_commandList);
  m_commandList->EndRecord();
}

}  // namespace Marbas::details