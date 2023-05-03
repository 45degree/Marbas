#include "RenderGraphBuilder.hpp"

#include <fstream>

#include "RenderGraph.hpp"

namespace Marbas {

RenderGraphGraphicsBuilder::RenderGraphGraphicsBuilder(details::RenderGraphGraphicsPass* pass, RenderGraph* graph)
    : m_graph(graph), m_pass(pass) {}

void
RenderGraphGraphicsBuilder::WriteTexture(const RenderGraphTextureHandler& handler, TextureAttachmentType type,
                                         int baseLayer, int LayerCount, int baseLevel, int levelCount) {
  auto& res = m_graph->m_resourceManager->m_graphTexture[handler.index];
  res.inputs.push_back(m_pass);
  m_pass->outputs.push_back(&res);
  m_pass->AddSubResDesc(type, handler, baseLayer, LayerCount, baseLevel, levelCount);
}

void
RenderGraphGraphicsBuilder::ReadTexture(const RenderGraphTextureHandler& handler, uintptr_t sampler, int baseLayer,
                                        int LayerCount, int baseLevel, int levelCount) {
  auto& res = m_graph->m_resourceManager->m_graphTexture[handler.index];
  res.outputs.push_back(m_pass);
  m_pass->inputs.push_back(&res);
  auto* desc = m_pass->AddInputAttachment<details::CombineImageDesc>();
  desc->m_sampler = sampler;
  desc->m_handler = handler;
  desc->m_baseLayer = baseLayer;
  desc->m_baseLevel = baseLevel;
  desc->m_layerCount = LayerCount;
  desc->m_levelCount = levelCount;
}

void
RenderGraphGraphicsBuilder::ReadStorageImage(const RenderGraphTextureHandler& handler, int baseLayer, int layerCount,
                                             int baseLevel, int levelCount) {
  auto& res = m_graph->m_resourceManager->m_graphTexture[handler.index];
  res.outputs.push_back(m_pass);
  m_pass->inputs.push_back(&res);

  auto* desc = m_pass->AddInputAttachment<details::StorageImageDesc>();
  desc->m_handler = handler;
  desc->m_baseLayer = baseLayer;
  desc->m_layerCount = layerCount;
  desc->m_baseLevel = baseLevel;
  desc->m_levelCount = levelCount;
}

void
RenderGraphGraphicsBuilder::SetFramebufferSize(uint32_t width, uint32_t height, uint32_t layer) {
  m_pass->m_framebufferWidth = width;
  m_pass->m_framebufferHeight = height;
  m_pass->m_framebufferLayer = layer;
}

void
RenderGraphGraphicsBuilder::EndPipeline() {
  m_pass->m_pipelineCreateInfos.push_back(m_pipelineCreateInfo);
}

}  // namespace Marbas
