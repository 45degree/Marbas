#include "RHI/OpenGL/OpenGLPipeline.hpp"

#include <glog/logging.h>

namespace Marbas {

static GLenum
ConvertToOpenGLType(ElementType type) {
  switch (type) {
    case ElementType::BYTE:
      return GL_BYTE;
    case ElementType::UNSIGNED_BYTE:
      return GL_UNSIGNED_BYTE;
    case ElementType::SHORT:
      return GL_SHORT;
    case ElementType::UNSIGNED_SHORT:
      return GL_UNSIGNED_SHORT;
    case ElementType::INT:
      return GL_INT;
    case ElementType::UNSIGNED_INT:
      return GL_UNSIGNED_INT;
    case ElementType::HALF_FLOAT:
      return GL_HALF_FLOAT;
    case ElementType::FLOAT:
      return GL_FLOAT;
    case ElementType::DOUBLE:
      return GL_DOUBLE;
  }
}

constexpr static GLenum
ConvertToOpenGLDepthCompareFunc(DepthCompareOp depthCompare) {
  switch (depthCompare) {
    case DepthCompareOp::ALWAYS:
      return GL_ALWAYS;
    case DepthCompareOp::NEVER:
      return GL_NEVER;
    case DepthCompareOp::LESS:
      return GL_LESS;
    case DepthCompareOp::EQUAL:
      return GL_EQUAL;
    case DepthCompareOp::LEQUAL:
      return GL_LEQUAL;
    case DepthCompareOp::GREATER:
      return GL_GREATER;
    case DepthCompareOp::NOTEQUAL:
      return GL_NOTEQUAL;
    case DepthCompareOp::GEQUAL:
      return GL_GEQUAL;
    default:
      return GL_LESS;
  }
}

OpenGLGraphicsPipeline::OpenGLGraphicsPipeline() { glCreateVertexArrays(1, &m_VAO); }

OpenGLGraphicsPipeline::~OpenGLGraphicsPipeline() { glDeleteVertexArrays(1, &m_VAO); }

void
OpenGLGraphicsPipeline::SetShader(const std::shared_ptr<Shader>& shader) {
  m_shader = std::dynamic_pointer_cast<OpenGLShader>(shader);
  LOG_IF(ERROR, m_shader == nullptr) << "shader is not a opengl shader";
}

void
OpenGLGraphicsPipeline::SetVertexBufferLayout(const Vector<ElementLayout>& vertexBufferLayout) {
  for (const auto& elementInfo : vertexBufferLayout) {
    GLuint index = elementInfo.index;
    auto size = static_cast<GLint>(elementInfo.count);
    GLboolean isNormalized = elementInfo.normalized;
    auto stride = static_cast<GLsizei>(elementInfo.stride);
    auto offset = elementInfo.offset;

    glEnableVertexArrayAttrib(m_VAO, index);
    switch (elementInfo.mateType) {
      case ElementType::BYTE:
      case ElementType::UNSIGNED_BYTE:
      case ElementType::SHORT:
      case ElementType::UNSIGNED_SHORT:
      case ElementType::INT:
      case ElementType::UNSIGNED_INT:
        glVertexArrayAttribIFormat(m_VAO, index, size, ConvertToOpenGLType(elementInfo.mateType),
                                   offset);
        break;
      case ElementType::FLOAT:
      case ElementType::DOUBLE:
      case ElementType::HALF_FLOAT:
        glVertexArrayAttribFormat(m_VAO, index, size, ConvertToOpenGLType(elementInfo.mateType),
                                  isNormalized, offset);
    }
    glVertexArrayAttribBinding(m_VAO, index, 0);
  }
}

void
OpenGLGraphicsPipeline::SetViewPort(const ViewportInfo& viewportInfo) {
  m_viewPortInfo = viewportInfo;
}

void
OpenGLGraphicsPipeline::SetScissorInfo(const ScissorInfo& scissorInfo) {
  m_scissorInfo = scissorInfo;
}

void
OpenGLGraphicsPipeline::SetRastreizationInfo(const RasterizationInfo& rasterizationInfo) {
  m_rasterizationInfo = rasterizationInfo;
}

void
OpenGLGraphicsPipeline::SetDepthStencilInfo(const DepthStencilInfo& depthStencilInfo) {
  m_depthStencilInfo = depthStencilInfo;
}

void
OpenGLGraphicsPipeline::SetMultisampleInfo(const MultisampleInfo& multisampleInfo) {
  m_multisampleInfo = multisampleInfo;
}

void
OpenGLGraphicsPipeline::SetBlendInfo(const BlendInfo& blendInfo) {
  m_blendInfo = blendInfo;
}

void
OpenGLGraphicsPipeline::SetDescriptorSetInfo(const DescriptorSetInfo& descriptorSetInfo) {
  m_descriptorSetInfo = descriptorSetInfo;
}

void
OpenGLGraphicsPipeline::Create() {}

void
OpenGLGraphicsPipeline::Bind() const {
  glViewport(m_viewPortInfo.x, m_viewPortInfo.y, m_viewPortInfo.width, m_viewPortInfo.height);
  // glScissor(m_scissorInfo.x, m_scissorInfo.y, m_scissorInfo.x, m_scissorInfo.y);
  if (m_depthStencilInfo.depthTestEnable) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(ConvertToOpenGLDepthCompareFunc(m_depthStencilInfo.depthCompareOp));
  } else {
    glDisable(GL_DEPTH_TEST);
  }

  m_shader->Use();
  glBindVertexArray(m_VAO);
}

}  // namespace Marbas
