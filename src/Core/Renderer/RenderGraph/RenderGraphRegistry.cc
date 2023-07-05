#include "RenderGraphRegistry.hpp"

#include "RenderGraph.hpp"

namespace Marbas {

RenderGraphGraphicsRegistry::RenderGraphGraphicsRegistry(RenderGraph* graph, Pass* pass, void* userData)
    : m_graph(graph), m_pass(pass), m_userData(userData) {}

uintptr_t
RenderGraphGraphicsRegistry::GetInputDescriptorSet() {
  return m_pass->m_descriptorSet;
}

uintptr_t
RenderGraphGraphicsRegistry::GetPipeline(size_t index) {
  return m_pass->m_pipelines[index];
}

FrameBuffer*
RenderGraphGraphicsRegistry::GetFrameBuffer() {
  return m_pass->m_framebuffer;
}

Image*
RenderGraphGraphicsRegistry::GetImage(RenderGraphTextureHandler handler) {
  auto& texture = m_graph->m_resourceManager->m_graphTexture[handler.index];
  return texture.GetImage();
}

// compute

RenderGraphComputeRegistry::RenderGraphComputeRegistry(RenderGraph* graph, Pass* pass, void* userData)
    : m_graph(graph), m_pass(pass), m_userData(userData) {}

uintptr_t
RenderGraphComputeRegistry::GetPipeline(size_t index) {
  return m_pass->m_pipelines[index];
}

uintptr_t
RenderGraphComputeRegistry::GetInputDescriptorSet() {
  return m_pass->m_descriptorSet;
}

Image*
RenderGraphComputeRegistry::GetImage(RenderGraphTextureHandler handler) {
  auto& texture = m_graph->m_resourceManager->m_graphTexture[handler.index];
  return texture.GetImage();
}

}  // namespace Marbas
