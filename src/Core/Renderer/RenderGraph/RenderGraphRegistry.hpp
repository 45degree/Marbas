#pragma once

#include "Core/Scene/Scene.hpp"
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
  RenderGraphRegistry(RenderGraph* graph, Scene* scene, details::RenderGraphGraphicsPass* pass);

  uintptr_t
  GetInputDescriptorSet();

  uintptr_t
  GetPipeline(size_t index);

  const Scene*
  GetCurrentActiveScene() const {
    return m_scene;
  }

  FrameBuffer*
  GetFrameBuffer();

 private:
  RenderGraph* m_graph;
  Scene* m_scene;
  details::RenderGraphGraphicsPass* m_pass;
};

}  // namespace Marbas
