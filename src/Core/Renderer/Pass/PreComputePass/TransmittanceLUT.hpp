#pragma once

#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"

namespace Marbas {

struct TransmittanceLUTPassCreateInfo {
  RenderGraphTextureHandler lutTexture;
  RHIFactory* rhiFactory;
  uint32_t width;
  uint32_t height;
};

class TransmittanceLUTPass final {
 public:
  TransmittanceLUTPass(const TransmittanceLUTPassCreateInfo& createInfo);
  ~TransmittanceLUTPass();

 public:
  void
  SetUp(RenderGraphGraphicsBuilder& builder);

  void
  Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList);

 private:
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

  DescriptorSetArgument m_argument;
  uintptr_t m_descriptorSet;

  uint32_t m_width;
  uint32_t m_height;
  RenderGraphTextureHandler m_lutTexture;
};

}  // namespace Marbas
