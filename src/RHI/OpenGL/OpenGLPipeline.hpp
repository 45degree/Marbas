#pragma once

#include "RHI/Interface/Pipeline.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"
#include "RHI/OpenGL/OpenGLShader.hpp"

namespace Marbas {

class OpenGLGraphicsPipeline : public GraphicsPipeLine {
 public:
  OpenGLGraphicsPipeline();
  ~OpenGLGraphicsPipeline();

  OpenGLGraphicsPipeline(const OpenGLGraphicsPipeline&) = delete;
  OpenGLGraphicsPipeline&
  operator=(const OpenGLGraphicsPipeline&) = delete;

 public:
  void
  SetShader(const std::shared_ptr<Shader>& shader) override;

  void
  SetVertexBufferLayout(const Vector<ElementLayout>& vertexBufferLayout) override;

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
  SetDescriptorSetInfo(const DescriptorSetInfo& descriptorSetInfo) override;

  void
  Create() override;

  void
  Bind() const;

 private:
  GLuint m_VAO;

  ViewportInfo m_viewPortInfo;
  ScissorInfo m_scissorInfo;
  RasterizationInfo m_rasterizationInfo;
  DepthStencilInfo m_depthStencilInfo;
  MultisampleInfo m_multisampleInfo;
  BlendInfo m_blendInfo;
  DescriptorSetInfo m_descriptorSetInfo;

  std::shared_ptr<OpenGLShader> m_shader = nullptr;
};

}  // namespace Marbas
