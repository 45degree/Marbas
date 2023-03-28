#pragma once

#include "RenderGraphResource.hpp"

namespace Marbas {

namespace details {
class RenderGraphPass;
class RenderGraphGraphicsPass;
};  // namespace details

class RenderGraph;

enum class TextureAttachmentType {
  COLOR,
  DEPTH,
  RESOLVE,
};

class RenderGraphPipelineCreateInfo {
  friend class RenderGraphGraphicsBuilder;
  friend class details::RenderGraphGraphicsPass;
  friend class GraphicsRenderCommandList;

 public:
  void
  SetPipelineLayout(const Vector<DescriptorSetLayoutBinding>& bindings) {
    m_bindings = bindings;
  }

  void
  AddShader(ShaderType shaderType, const Path& shaderPath);

  void
  SetMultiSamples(SampleCount sampleCount) {
    m_sampleCount = sampleCount;
  }

  void
  SetColorAttachmentsDesc(const Vector<ColorTargetDesc>& desc) {
    m_colorAttachments = desc;
  }

  void
  SetDepthAttachmentDesc(const DepthTargetDesc& desc) {
    m_depthAttachments = desc;
  }

  void
  SetResolveAttachmentDesc(const Vector<ResolveTargetDesc>& desc) {
    m_resolveAttachments = desc;
  }

  void
  SetVertexInputElementDesc(const Vector<InputElementDesc>& desc) {
    m_inputElementDesc = desc;
  }

  void
  SetVertexInputElementView(const Vector<InputElementView>& desc) {
    m_inputElementView = desc;
  }

  void
  SetBlendAttachments(const Vector<BlendAttachment>& attachment) {
    m_blendAttachment = attachment;
  }

  void
  SetBlendConstance(float r, float g, float b, float a) {
    m_blendConstance = {r, g, b, a};
  }

  void
  SetDepthStencil(const DepthStencilCreateInfo& createInfo) {
    m_depthStencilCreateInfo = createInfo;
  }

 private:
  Vector<DescriptorSetLayoutBinding> m_bindings;
  Vector<ShaderStageCreateInfo> m_shaders;
  SampleCount m_sampleCount = SampleCount::BIT1;
  Vector<ColorTargetDesc> m_colorAttachments;
  std::optional<DepthTargetDesc> m_depthAttachments;
  Vector<ResolveTargetDesc> m_resolveAttachments;
  std::array<float, 4> m_blendConstance;
  Vector<BlendAttachment> m_blendAttachment;
  Vector<InputElementDesc> m_inputElementDesc;
  Vector<InputElementView> m_inputElementView;
  RasterizationCreateInfo m_rasterizationCreateInfo;
  DepthStencilCreateInfo m_depthStencilCreateInfo;
};

class RenderGraphGraphicsBuilder final {
 public:
  RenderGraphGraphicsBuilder(details::RenderGraphGraphicsPass* pass, RenderGraph* graph);
  virtual ~RenderGraphGraphicsBuilder() = default;

  void
  SetFramebufferSize(uint32_t width, uint32_t height, uint32_t layer);

  void
  WriteTexture(const RenderGraphTextureHandler& handler, TextureAttachmentType type = TextureAttachmentType::COLOR,
               int baseLayer = 0, int LayerCount = 1, int baseLevel = 0, int levelCount = 1);

  void
  ReadTexture(const RenderGraphTextureHandler& handler);

  void
  SetPipelineInfo(const RenderGraphPipelineCreateInfo& createInfo);

 private:
  RenderGraph* m_graph;
  details::RenderGraphGraphicsPass* m_pass;
};

}  // namespace Marbas
