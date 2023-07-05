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
  RenderGraphGraphicsRegistry(RenderGraph* graph, Pass* pass, void* userData);

  uintptr_t
  GetInputDescriptorSet();

  uintptr_t
  GetPipeline(size_t index);

  void*
  GetUserData() {
    return m_userData;
  }

  FrameBuffer*
  GetFrameBuffer();

  Image*
  GetImage(RenderGraphTextureHandler handler);

 private:
  RenderGraph* m_graph;
  void* m_userData;
  Pass* m_pass;
};

class RenderGraphComputeRegistry final {
  using Pass = details::RenderGraphComputePass;

 public:
  RenderGraphComputeRegistry(RenderGraph* graph, Pass* pass, void* userData);

  uintptr_t
  GetInputDescriptorSet();

  uintptr_t
  GetPipeline(size_t index);

  void*
  GetUserData() {
    return m_userData;
  }

  Image*
  GetImage(RenderGraphTextureHandler handler);

 private:
  RenderGraph* m_graph;
  void* m_userData = nullptr;
  Pass* m_pass;
};

}  // namespace Marbas
