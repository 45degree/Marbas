#pragma once

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"

namespace Marbas::GI {

struct LightInjectPassCreateInfo {
  RHIFactory* rhiFactory;
};

class LightInjectPass final {
 public:
  LightInjectPass(const LightInjectPassCreateInfo& createInfo);
  ~LightInjectPass();

 public:
  void
  SetUp(RenderGraphComputeBuilder& builder);

  void
  Execute(RenderGraphComputeRegistry& registry, ComputeCommandBuffer& commandBuffer);

  bool
  IsEnable(RenderGraphComputeRegistry& registry);

 private:
  RHIFactory* m_rhiFactory = nullptr;
};

}  // namespace Marbas::GI
