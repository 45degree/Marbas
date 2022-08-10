#pragma once

#include <memory>

#include "DescriptorSet.hpp"
#include "RHI/Interface/Shader.hpp"

namespace Marbas {

struct ViewportInfo {
  uint32_t x;
  uint32_t y;
  uint32_t width;
  uint32_t height;
  uint32_t minDepth;
  uint32_t maxDepth;
};

struct ScissorInfo {
  uint32_t x;
  uint32_t y;
  uint32_t width;
  uint32_t height;
};

enum class PolygonMode {

};

enum class CullMode {

};

enum class FrontFace {

};

struct RasterizationInfo {
  bool depthClampEnable;         // TODO
  bool rasterizerDiscardEnable;  // TODO
  PolygonMode polygonMode;
  float lineWidth = 1.0f;
  CullMode cullMode;
  FrontFace frontFace;
  bool depthBiasEnable;  // TODO
};

enum class DepthCompareOp {
  ALWAYS,
  NEVER,
  LESS,
  EQUAL,
  LEQUAL,
  GREATER,
  NOTEQUAL,
  GEQUAL,
};

struct DepthStencilInfo {
  bool depthTestEnable = true;
  bool depthWriteEnable = true;
  DepthCompareOp depthCompareOp = DepthCompareOp::LESS;
  bool depthBoundsTestEnable = true;
  bool stencilTestEnable = true;
};

struct MultisampleInfo {
  bool sampleShadingEnable = false;
  // rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
};

enum class BlendOp {

};

struct BlendInfo {
  bool blendEnable;
  BlendOp logicOp;
};

struct GraphicsPipeLineCreateInfo {
  // VertexInputDescription vertexInputDescription;
  ViewportInfo viewport;
  ScissorInfo scissor;
  DescriptorSetInfo descriptorsSetInfo;
  MultisampleInfo multisampleInfo;
};

enum class VertexInputRate {
  VERTEX = 0,
  INSTANCE = 1,
};

class GraphicsPipeLine {
 protected:
  using BindingDivisorInfo = std::tuple<uint32_t, uint32_t>;

 public:
  virtual void
  SetShader(const std::shared_ptr<Shader>& shader) = 0;

  virtual void
  SetVertexBufferLayout(const Vector<ElementLayout>& vertexBufferLayout, VertexInputRate rate) = 0;

  virtual void
  SetVertexInputBindingDivisor(const Vector<BindingDivisorInfo>& divisorDescription) = 0;

  virtual void
  SetViewPort(const ViewportInfo& viewportInfo) = 0;

  virtual void
  SetScissorInfo(const ScissorInfo& scissorInfo) = 0;

  virtual void
  SetRastreizationInfo(const RasterizationInfo& rasterizationInfo) = 0;

  virtual void
  SetDepthStencilInfo(const DepthStencilInfo& depthStencilInfo) = 0;

  virtual void
  SetMultisampleInfo(const MultisampleInfo& multisampleInfo) = 0;

  virtual void
  SetBlendInfo(const BlendInfo& blendInfo) = 0;

  virtual void
  SetDescriptorSetInfo(const DescriptorSetInfo& descriptorSetInfo) = 0;

  /**
   * @brief Create the pipeline, if create failed, throw an exception
   */
  virtual void
  Create() = 0;
};

}  // namespace Marbas
