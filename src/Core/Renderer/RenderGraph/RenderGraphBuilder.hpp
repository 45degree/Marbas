#pragma once

#include "RenderGraphResource.hpp"

namespace Marbas {

namespace details {
class RenderGraphPass;
class RenderGraphGraphicsPass;
class RenderGraphComputePass;
};  // namespace details

class RenderGraph;

enum class TextureAttachmentType {
  COLOR,
  DEPTH,
  RESOLVE,
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
  ReadTexture(const RenderGraphTextureHandler& handler, uintptr_t sampler, int baseLayer = 0, int layerCount = 1,
              int baseLevel = 0, int levelCount = 1);

  void
  ReadStorageImage(const RenderGraphTextureHandler& handler, int baseLayer = 0, int layerCount = 1, int baseLevel = 0,
                   int levelCount = 1);

  void
  BeginPipeline() {
    m_pipelineCreateInfo = {};
  }

  void
  EndPipeline();

  void
  EnableDepthTest(bool isEnable) {
    m_pipelineCreateInfo.depthStencilInfo.depthTestEnable = isEnable;
  }

  void
  EnableDepthWrite(bool isEnable) {
    m_pipelineCreateInfo.depthStencilInfo.depthWriteEnable = isEnable;
  }

  void
  SetDepthCampareOp(DepthCompareOp depthOp) {
    m_pipelineCreateInfo.depthStencilInfo.depthCompareOp = depthOp;
  }

  void
  SetPolygonMode(PolygonMode mode) {
    m_pipelineCreateInfo.rasterizationInfo.polygonMode = mode;
  }

  void
  SetPrimitiveType(PrimitiveTopology type) {
    m_pipelineCreateInfo.inputAssemblyState.topology = type;
  }

  void
  SetCullMode(CullMode mode) {
    m_pipelineCreateInfo.rasterizationInfo.cullMode = mode;
  }

  void
  SetFrontFace(FrontFace frontFace) {
    m_pipelineCreateInfo.rasterizationInfo.frontFace = frontFace;
  }

  void
  AddShaderArgument(const DescriptorSetArgument& argument) {
    m_pipelineCreateInfo.layout.push_back(argument);
  }

  void
  AddShader(const std::filesystem::path& spirvPath, ShaderType type) {
    m_pipelineCreateInfo.shaderStageCreateInfo.push_back(ShaderStageCreateInfo(spirvPath, type));
  }

  void
  AddColorTarget(const ColorTargetDesc& desc) {
    m_pipelineCreateInfo.outputRenderTarget.colorAttachments.push_back(desc);
  }

  void
  SetDepthTarget(const DepthTargetDesc& desc) {
    m_pipelineCreateInfo.outputRenderTarget.depthAttachments = desc;
  }

  void
  AddResolveTarget(const ResolveTargetDesc& desc) {
    m_pipelineCreateInfo.outputRenderTarget.resolveAttachments.push_back(desc);
  }

  void
  SetBlendConstant(float r, float g, float b, float a) {
    m_pipelineCreateInfo.blendInfo.constances = {r, g, b, a};
  }

  void
  AddBlendAttachments(const BlendAttachment& attachment) {
    m_pipelineCreateInfo.blendInfo.attachments.push_back(attachment);
  }

  void
  SetVertexInputElementDesc(const std::vector<InputElementDesc>& elementDesc) {
    m_pipelineCreateInfo.vertexInputLayout.elementDesc = elementDesc;
  }

  void
  SetVertexInputElementView(const std::vector<InputElementView>& elementView) {
    m_pipelineCreateInfo.vertexInputLayout.viewDesc = elementView;
  }

  void
  SetPushConstantSize(uint32_t size = 0) {
    m_pipelineCreateInfo.pushConstantSize = size;
  }

 private:
  GraphicsPipeLineCreateInfo m_pipelineCreateInfo;
  RenderGraph* m_graph;
  details::RenderGraphGraphicsPass* m_pass;
};

class RenderGraphComputeBuilder final {
  using Pass = details::RenderGraphComputePass;
  using TextureHandler = RenderGraphTextureHandler;

 public:
  RenderGraphComputeBuilder(Pass* pass, RenderGraph* graph);
  ~RenderGraphComputeBuilder();

 public:
  void
  ReadTexture(const TextureHandler& handler, uintptr_t sampler, int baseLayer = 0, int layerCount = 1,
              int baseLevel = 0, int levelCount = 1);

  void
  ReadStorageImage(const TextureHandler& handler, int baseLayer = 0, int layerCount = 1, int baseLevel = 0,
                   int levelCount = 1);

  void
  AddShaderArgument(const DescriptorSetArgument& argument) {
    m_pipelineCreateInfo.layout.push_back(argument);
  }

  void
  SetPushConstantSize(uint32_t size = 0) {
    m_pipelineCreateInfo.pushConstantSize = size;
  }

  void
  SetShader(const std::filesystem::path& spirvPath) {
    m_pipelineCreateInfo.computeShaderStage = ShaderStageCreateInfo(spirvPath, ShaderType::COMPUTE_SHADER);
  }

  void
  BeginPipeline() {
    m_pipelineCreateInfo = {};
  }

  void
  EndPipeline();

 private:
  ComputePipelineCreateInfo m_pipelineCreateInfo;
  Pass* m_pass;
  RenderGraph* m_graph;
};

}  // namespace Marbas
