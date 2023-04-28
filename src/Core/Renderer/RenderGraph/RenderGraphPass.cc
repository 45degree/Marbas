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

  for (auto pipeline : m_pipelines) {
    pipelineCtx->DestroyPipeline(pipeline);
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
RenderGraphGraphicsPass::AddInputAttachment(RenderGraphTextureHandler handler, uintptr_t sampler, uint32_t baseLayer,
                                            uint32_t layerCount, uint32_t baseLevel, uint32_t levelCount) {
  m_inputAttachmentSampler.push_back(sampler);
  m_inputAttachment.push_back({handler, baseLayer, layerCount, baseLevel, levelCount});
}

void
RenderGraphGraphicsPass::Initialize(RenderGraph* graph) {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto bufCtx = m_rhiFactory->GetBufferContext();

  auto GetImageViewFromDesc = [&graph](const SubResourceDesc& desc) -> ImageView* {
    if (desc.handler == std::nullopt) return nullptr;
    DLOG_ASSERT((*desc.handler).index < graph->m_resourceManager->m_graphTexture.size())
        << FORMAT("resource {} is not create", (*desc.handler).index);

    auto& res = graph->m_resourceManager->m_graphTexture[(*desc.handler).index];
    return res.GetImageView(desc.baseLayer, desc.layerCount, desc.baseLevel, desc.levelCount);
  };

  m_commandBuffer = bufCtx->CreateGraphicsCommandBuffer();

  /**
   * create pipeline
   */
  for (int i = 0; i < m_pipelineCreateInfos.size(); i++) {
    auto pipeline = pipelineCtx->CreatePipeline(m_pipelineCreateInfos[i]);
    m_pipelines.push_back(pipeline);
  }

  /**
   * create descriptorSet and bind image view
   */
  if (!m_inputAttachment.empty()) {
    DescriptorSetArgument argument;
    for (int i = 0; i < m_inputAttachment.size(); i++) {
      argument.Bind(i, DescriptorType::IMAGE);  // all input attachment is from an image
    }
    m_descriptorSet = pipelineCtx->CreateDescriptorSet(argument);
    for (uint16_t i = 0; i < m_inputAttachment.size(); i++) {
      BindImageInfo bindInfo{
          .descriptorSet = m_descriptorSet,
          .bindingPoint = i,
          .imageView = GetImageViewFromDesc(m_inputAttachment[i]),
          .sampler = m_inputAttachmentSampler[i],
      };
      pipelineCtx->BindImage(bindInfo);
    }
  }

  /**
   * create framebuffer
   */

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
  // TODO: change this
  framebufferCreateInfo.pipeline = m_pipelines[0];
  m_framebuffer = pipelineCtx->CreateFrameBuffer(framebufferCreateInfo);
}

LambdaGraphicsRenderGraphPass::LambdaGraphicsRenderGraphPass(StringView name, RHIFactory* rhiFactory)
    : RenderGraphGraphicsPass(name, rhiFactory) {}

void
LambdaGraphicsRenderGraphPass::Execute(RenderGraph* graph, Scene* scene) {
  RenderGraphRegistry registry(graph, scene, this);
  m_command(registry, *m_commandBuffer);
}

}  // namespace Marbas::details
