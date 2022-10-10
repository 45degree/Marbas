#pragma once

#include <memory>

#include "DescriptorSet.hpp"
#include "RHI/Interface/Shader.hpp"

namespace Marbas {

struct ViewportInfo {
  uint32_t x = 0;
  uint32_t y = 0;
  uint32_t width;
  uint32_t height;
  uint32_t minDepth = 0;
  uint32_t maxDepth = 1;
};

struct ScissorInfo {
  uint32_t x;
  uint32_t y;
  uint32_t width;
  uint32_t height;
};

enum class PolygonMode {
  FILL,
  POINT,
  LINE,
};

enum class CullMode {
  NONE,
  FRONT,
  BACK,
  FRONT_AND_BACK,
};

enum class FrontFace {
  CW,
  CCW,
};

struct RasterizationInfo {
  bool depthClampEnable = false;
  bool rasterizerDiscardEnable = false;
  PolygonMode polygonMode = PolygonMode::FILL;
  float lineWidth = 1.0f;
  CullMode cullMode = CullMode::NONE;
  FrontFace frontFace = FrontFace::CCW;
  bool depthBiasEnable = false;
  float depthBiasConstantFactor = 0.0f;
  float depthBiasClamp = 0.0f;
  float depthBiasSlopeFactor = 0.0f;
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
};

enum class BlendOp {
  ADD,               // result = src + dst
  SUBTRACT,          // result = src - dst
  REVERSE_SUBTRACT,  // resutl = dst - src
};

enum class LogicOp {
  CLEAR,          // 0
  SET,            // 1
  COPY,           // S
  COPY_INVERTED,  //~S
  NOOP,           // D
  INVERT,         // ~D
  AND,            // S & D
  NAND,           // ~(S & D)
  OR,             // S | D
  NOR,            // ~(S | D)
  XOR,            // S ^ D
  EQUIV,          // ~(S ^ D)
  AND_REVERSE,    // S & ~D
  AND_INVERTED,   // ~S & D
  OR_REVERSE,     // S | ~D
  OR_INVERTED,    // ~S | D
};

enum class BlendFactor {
  ZERO,                      // factor equal 0
  ONE,                       // factor equal 1
  SRC_COLOR,                 // factor equal src
  ONE_MINUS_SRC_COLOR,       // factor equal 1 - src
  DST_COLOR,                 // factor equal dst
  ONE_MINUS_DST_COLOR,       // factor equal 1 − dst
  SRC_ALPHA,                 // factor equal alpha's value of src
  ONE_MINUS_SRC_ALPHA,       // factor equal 1 - alpha's value of src
  DST_ALPHA,                 // factor equal alpha's value of dst
  ONE_MINUS_DST_ALPHA,       // factor equal 1 - alpha's value of dst
  CONSTANT_COLOR,            // factor equal a const
  ONE_MINUS_CONSTANT_COLOR,  // factor equal 1 - the const vaule
  CONSTANT_ALPHA,            // factor equal alpha's value of the const
  ONE_MINUS_CONSTANT_ALPHA,  // factor equal 1 - alpha's value of the const
};

struct BlendAttachment {
  bool blendEnable = false;
  BlendFactor srcColorBlendFactor = BlendFactor::SRC_ALPHA;
  BlendFactor dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
  BlendOp colorBlendOp = BlendOp::ADD;
  BlendFactor srcAlphaBlendFactor = BlendFactor::SRC_ALPHA;
  BlendFactor dstAlphaBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
  BlendOp alphaBlendOp = BlendOp::ADD;
};

struct BlendInfo {
  bool logicOpEnable = false;
  LogicOp logicOp = LogicOp::COPY;
  Vector<BlendAttachment> attachments;
  std::array<float, 4> constances = {0, 0, 0, 0};
};

struct GraphicsPipeLineCreateInfo {
  // VertexInputDescription vertexInputDescription;
  ViewportInfo viewport;
  ScissorInfo scissor;
  DescriptorSetLayout descriptorsSetInfo;
  MultisampleInfo multisampleInfo;
};

struct GraphicsPipeLineLayout {
  DescriptorSetLayout descriptorSetLayout;
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
  SetPipelineLayout(const GraphicsPipeLineLayout& pipelineLayout) = 0;

  /**
   * @brief Create the pipeline, if create failed, throw an exception
   */
  virtual void
  Create() = 0;
};

}  // namespace Marbas
