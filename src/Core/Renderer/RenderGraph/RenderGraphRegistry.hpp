#pragma once

#include "Core/Scene/Scene.hpp"
#include "RHIFactory.hpp"
#include "RenderGraphResource.hpp"

namespace Marbas {

namespace details {
class RenderGraphPass;
class RenderGraphGraphicsPass;
class RenderGraphComputePass;
};  // namespace details

class RenderGraph;
class RenderGraphGraphicsRegistry final {
  using Pass = details::RenderGraphGraphicsPass;

 public:
  RenderGraphGraphicsRegistry(RenderGraph* graph, Scene* scene, Pass* pass);

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

  Image*
  GetImage(RenderGraphTextureHandler handler);

 private:
  RenderGraph* m_graph;
  Scene* m_scene;
  Pass* m_pass;
};

class RenderGraphComputeRegistry final {
  using Pass = details::RenderGraphComputePass;

 public:
  RenderGraphComputeRegistry(RenderGraph* graph, Scene* scene, Pass* pass);

  uintptr_t
  GetInputDescriptorSet();

  uintptr_t
  GetPipeline(size_t index);

  const Scene*
  GetCurrentActiveScene() const {
    return m_scene;
  }

  Image*
  GetImage(RenderGraphTextureHandler handler);

 private:
  RenderGraph* m_graph;
  Scene* m_scene;
  Pass* m_pass;
};

}  // namespace Marbas
