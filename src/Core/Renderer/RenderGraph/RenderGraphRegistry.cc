#include "RenderGraphRegistry.hpp"

#include "RenderGraph.hpp"

namespace Marbas {

RenderGraphRegistry::RenderGraphRegistry(RenderGraph* graph) : m_graph(graph) {}

Image*
RenderGraphRegistry::GetRenderBackendTexture(const RenderGraphTextureHandler& handler) {
  auto& res = m_graph->m_resourceManager->m_graphTexture[handler.index];
  return res.GetImage();
}

ImageView*
RenderGraphRegistry::GetRenderBackendTextureSubResource(const RenderGraphTextureHandler& handler, uint32_t baseLayer,
                                                        uint32_t layerCount, uint32_t baseLevel, uint32_t levelCount) {
  auto& res = m_graph->m_resourceManager->m_graphTexture[handler.index];
  return res.GetImageView(baseLayer, layerCount, baseLevel, levelCount);
}

}  // namespace Marbas
