#pragma once

#include "RHI/Interface/Pipeline.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"
#include "RHI/OpenGL/OpenGLShader.hpp"

namespace Marbas {

class OpenGLGraphicsPipeline final : public GraphicsPipeLine {
 public:
  OpenGLGraphicsPipeline();
  virtual ~OpenGLGraphicsPipeline();

  OpenGLGraphicsPipeline(const OpenGLGraphicsPipeline&) = delete;
  OpenGLGraphicsPipeline&
  operator=(const OpenGLGraphicsPipeline&) = delete;

 public:
  void
  SetShader(const std::shared_ptr<Shader>& shader) override;

  void
  SetVertexBufferLayout(const Vector<ElementLayout>& vertexBufferLayout,
                        VertexInputRate rate) override;

  VertexInputRate
  GetVertexInputRate() const {
    return m_vertexInputRate;
  }

  void
  SetVertexInputBindingDivisor(const Vector<BindingDivisorInfo>& divisorDescription) override;

  void
  SetViewPort(const ViewportInfo& viewportInfo) override;

  void
  SetScissorInfo(const ScissorInfo& scissorInfo) override;

  void
  SetRastreizationInfo(const RasterizationInfo& rasterizationInfo) override;

  void
  SetDepthStencilInfo(const DepthStencilInfo& depthStencilInfo) override;

  void
  SetMultisampleInfo(const MultisampleInfo& multisampleInfo) override;

  void
  SetBlendInfo(const BlendInfo& blendInfo) override;

  void
  SetPipelineLayout(const GraphicsPipeLineLayout& pipelineLayout) override;

  void
  Create() override;

  void
  Bind() const;

  void
  UnBind() const;

 private:
  GLuint m_VAO;

  ViewportInfo m_viewPortInfo;
  ScissorInfo m_scissorInfo;
  RasterizationInfo m_rasterizationInfo;
  DepthStencilInfo m_depthStencilInfo;
  MultisampleInfo m_multisampleInfo;
  BlendInfo m_blendInfo;
  DescriptorSetLayout m_descriptorSetInfo;

  Vector<ElementLayout> m_vertexBufferLayout;
  Vector<BindingDivisorInfo> m_bindingDivisorInfo;
  VertexInputRate m_vertexInputRate;

  std::shared_ptr<OpenGLShader> m_shader = nullptr;
};

}  // namespace Marbas
