#pragma once

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"

namespace Marbas {

struct MultiScatterLUTCreateInfo {
  RHIFactory* rhiFactory = nullptr;
  uint32_t height;
  uint32_t width;
  RenderGraphTextureHandler transmittanceLUT;
  RenderGraphTextureHandler multiScatterLUT;
};

class MultiScatterLUT final {
 public:
  MultiScatterLUT(const MultiScatterLUTCreateInfo& createInfo);
  ~MultiScatterLUT();

 public:
  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphRegistry& registry, GraphicsCommandBuffer& commandList);

 private:
  uint32_t m_width;
  uint32_t m_height;
  RHIFactory* m_rhiFactory = nullptr;
  struct AtmosphereInfo {
    float atmosphereHeight = 60000;
    float rayleighScalarHeight = 8500;
    float mieScalarHeight = 1200;
    float mieAnisotropy = 0.5;  // [-1, 1]
    float planetRadius = 6360000;
    float ozoneCenterHeight = 25000;
    float ozoneWidth = 15000;
  } m_atmosphereInfo;
  Buffer* m_atmosphereInfoBuffer = nullptr;
  uintptr_t m_sampler;

  DescriptorSetArgument m_argument;
  uintptr_t m_descriptorSet;
  DescriptorSetArgument m_transmittanceLUTArgument;

  RenderGraphTextureHandler m_transmittanceLUT;
  RenderGraphTextureHandler m_multiScatterLUT;
};

}  // namespace Marbas
