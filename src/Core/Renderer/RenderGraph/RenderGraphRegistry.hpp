#pragma once

#include "RHIFactory.hpp"
#include "RenderGraphResource.hpp"

namespace Marbas {

namespace details {
class RenderGraphPass;
class RenderGraphGraphicsPass;
};  // namespace details

class RenderGraph;
class RenderGraphRegistry final {
 public:
  RenderGraphRegistry(RenderGraph* graph, details::RenderGraphGraphicsPass* pass);

  uintptr_t
  GetInputDescriptorSet();

  uintptr_t
  GetPipeline(size_t index);

  FrameBuffer*
  GetFrameBuffer();

 private:
  RenderGraph* m_graph;
  details::RenderGraphGraphicsPass* m_pass;
};

}  // namespace Marbas
