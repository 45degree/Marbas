#pragma once

#include "RHIFactory.hpp"
#include "RenderGraphResource.hpp"

namespace Marbas {

class RenderGraph;
class RenderGraphRegistry final {
 public:
  RenderGraphRegistry(RenderGraph* graph);

  Image*
  GetRenderBackendTexture(const RenderGraphTextureHandler& handler);

  ImageView*
  GetRenderBackendTextureSubResource(const RenderGraphTextureHandler& handler, uint32_t baseLayer, uint32_t LayerCount,
                                     uint32_t baseLevel, uint32_t LevelCount);

 private:
  RenderGraph* m_graph;
};

}  // namespace Marbas
