#include "RenderGraphRegistry.hpp"

#include "RenderGraph.hpp"

namespace Marbas {

RenderGraphRegistry::RenderGraphRegistry(RenderGraph* graph, Scene* scene, details::RenderGraphGraphicsPass* pass)
    : m_graph(graph), m_pass(pass), m_scene(scene) {}

uintptr_t
RenderGraphRegistry::GetInputDescriptorSet() {
  return m_pass->m_descriptorSet;
}

uintptr_t
RenderGraphRegistry::GetPipeline(size_t index) {
  return m_pass->m_pipelines[index];
}

FrameBuffer*
RenderGraphRegistry::GetFrameBuffer() {
  return m_pass->m_framebuffer;
}

}  // namespace Marbas
