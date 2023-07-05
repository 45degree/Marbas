#include "RenderGraphPass.hpp"

#include <glog/logging.h>

#include "RenderGraph.hpp"

namespace Marbas::details {

/**
 * input and output desc
 */

ImageView*
ImageDesc::GetImageView(RenderGraph* graph) const {
  auto& res = graph->m_resourceManager->m_graphTexture[m_handler.index];
  return res.GetImageView(m_baseLayer, m_layerCount, m_baseLevel, m_levelCount);
}

void
CombineImageDesc::Bind(RenderGraph* graph, PipelineContext* ctx, uintptr_t set, uint16_t bindingPoint) const {
  BindImageInfo bindInfo;
  auto* imageView = GetImageView(graph);
  bindInfo.imageView = imageView;
  bindInfo.sampler = m_sampler;
  bindInfo.bindingPoint = bindingPoint;
  bindInfo.descriptorSet = set;
  ctx->BindImage(bindInfo);
}

void
StorageImageDesc::Bind(RenderGraph* graph, PipelineContext* ctx, uintptr_t set, uint16_t bindingPoint) const {
  BindStorageImageInfo bindInfo;
  bindInfo.descriptorSet = set;
  bindInfo.bindingPoint = bindingPoint;
  bindInfo.imageView = GetImageView(graph);
  ctx->BindStorageImage(bindInfo);
  return;
}

/**
 * pass
 */

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
  auto desc = std::make_unique<CombineImageDesc>();
  desc->m_handler = handler;
  desc->m_baseLayer = baseLayer;
  desc->m_baseLevel = baseLevel;
  desc->m_layerCount = layerCount;
  desc->m_levelCount = levelCount;

  switch (type) {
    case TextureAttachmentType::COLOR:
      m_colorAttachment.push_back(std::move(desc));
      break;
    case TextureAttachmentType::DEPTH:
      m_depthAttachment = std::move(desc);
      break;
    case TextureAttachmentType::RESOLVE:
      m_resolveAttachment.push_back(std::move(desc));
      break;
  }
}

void
RenderGraphGraphicsPass::Initialize(RenderGraph* graph) {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto bufCtx = m_rhiFactory->GetBufferContext();

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
      m_inputAttachment[i]->SetArgument(argument, i);
    }
    m_descriptorSet = pipelineCtx->CreateDescriptorSet(argument);
    for (uint16_t i = 0; i < m_inputAttachment.size(); i++) {
      m_inputAttachment[i]->Bind(graph, pipelineCtx, m_descriptorSet, i);
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
    colorAttachment.push_back(attachment->GetImageView(graph));
  }

  if (m_depthAttachment != nullptr) {
    depthAttachment = m_depthAttachment->GetImageView(graph);
  }

  for (auto& attachment : m_resolveAttachment) {
    resolveAttachment.push_back(attachment->GetImageView(graph));
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

RenderGraphComputePass::RenderGraphComputePass(std::string_view name, RHIFactory* rhiFactory)
    : RenderGraphPass(name, rhiFactory) {}

void
RenderGraphComputePass::Initialize(RenderGraph* graph) {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto bufCtx = m_rhiFactory->GetBufferContext();

  m_commandBuffer = bufCtx->CreateComputeCommandBuffer();

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
      m_inputAttachment[i]->SetArgument(argument, i);
    }
    m_descriptorSet = pipelineCtx->CreateDescriptorSet(argument);
    for (uint16_t i = 0; i < m_inputAttachment.size(); i++) {
      m_inputAttachment[i]->Bind(graph, pipelineCtx, m_descriptorSet, i);
    }
  }
}

RenderGraphComputePass::~RenderGraphComputePass() {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  for (auto pipeline : m_pipelines) {
    pipelineCtx->DestroyPipeline(pipeline);
  }
}

LambdaGraphicsRenderGraphPass::LambdaGraphicsRenderGraphPass(StringView name, RHIFactory* rhiFactory)
    : RenderGraphGraphicsPass(name, rhiFactory) {}

void
LambdaGraphicsRenderGraphPass::Execute(RenderGraph* graph, void* userData) {
  RenderGraphGraphicsRegistry registry(graph, this, userData);
  m_command(registry, *m_commandBuffer);
}

LambdaComputeRenderGraphPass::LambdaComputeRenderGraphPass(StringView name, RHIFactory* rhiFactory)
    : RenderGraphComputePass(name, rhiFactory) {}

void
LambdaComputeRenderGraphPass::Execute(RenderGraph* graph, void* userData) {
  RenderGraphComputeRegistry registry(graph, this, userData);
  m_command(registry, *m_commandBuffer);
}

}  // namespace Marbas::details
