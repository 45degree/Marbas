#include "RHI/OpenGL/OpenGLPipeline.hpp"

#include <glog/logging.h>

#include "RHI/Interface/Pipeline.hpp"

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

constexpr static GLenum
ConvertToOpenGLLogicOp(LogicOp logicOp) {
  // clang-format off
  switch (logicOp) {
    case LogicOp::CLEAR:
      return GL_CLEAR;
    case LogicOp::SET:
      return GL_SET;
    case LogicOp::INVERT:
      return GL_INVERT;
    [[likely]] case LogicOp::COPY:
      return GL_COPY;
    case LogicOp::COPY_INVERTED:
      return GL_COPY_INVERTED;
    case LogicOp::AND:
      return GL_AND;
    case LogicOp::AND_INVERTED:
      return GL_AND_INVERTED;
    case LogicOp::AND_REVERSE:
      return GL_AND_INVERTED;
    case LogicOp::OR:
      return GL_OR;
    case LogicOp::OR_INVERTED:
      return GL_OR_INVERTED;
    case LogicOp::OR_REVERSE:
      return GL_OR_REVERSE;
    case LogicOp::XOR:
      return GL_XOR;
    case LogicOp::EQUIV:
      return GL_EQUIV;
    case LogicOp::NOOP:
      return GL_NOOP;
    case LogicOp::NOR:
      return GL_NOR;
    case LogicOp::NAND:
      return GL_NAND;
  }
  return GL_COPY;
  // clang-format on
}

constexpr static GLenum
ConvertToOpenGLBlendEquation(BlendOp op) {
  // clang-format off
  switch (op) {
    [[likely]] case BlendOp::ADD:
      return GL_FUNC_ADD;
    case BlendOp::SUBTRACT:
      return GL_FUNC_SUBTRACT;
    case BlendOp::REVERSE_SUBTRACT:
      return GL_FUNC_REVERSE_SUBTRACT;
  }
  // clang-format on
  return GL_FUNC_ADD;
}

constexpr static GLenum
ConvertToOpenGLBlendFuctor(BlendFactor factor) {
  // clang-format off
  switch (factor) {
    [[likely]] case BlendFactor::ONE:
      return GL_ONE;
    [[likely]] case BlendFactor::ZERO:
      return GL_ZERO;
    case BlendFactor::SRC_COLOR:
      return GL_SRC_COLOR;
    case BlendFactor::ONE_MINUS_SRC_COLOR:
      return GL_ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::DST_COLOR:
      return GL_DST_COLOR;
    case BlendFactor::ONE_MINUS_DST_COLOR:
      return GL_ONE_MINUS_DST_COLOR;
    [[likely]] case BlendFactor::SRC_ALPHA:
      return GL_SRC_ALPHA;
    [[likely]] case BlendFactor::ONE_MINUS_SRC_ALPHA:
      return GL_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DST_ALPHA:
      return GL_DST_ALPHA;
    case BlendFactor::ONE_MINUS_DST_ALPHA:
      return GL_ONE_MINUS_DST_ALPHA;
    case BlendFactor::CONSTANT_COLOR:
      return GL_CONSTANT_COLOR;
    case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
      return GL_ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::CONSTANT_ALPHA:
      return GL_CONSTANT_ALPHA;
    case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
      return GL_ONE_MINUS_CONSTANT_ALPHA;
  }
  // clang-format on
  return GL_SRC_ALPHA;
}

OpenGLGraphicsPipeline::OpenGLGraphicsPipeline() { glCreateVertexArrays(1, &m_VAO); }

OpenGLGraphicsPipeline::~OpenGLGraphicsPipeline() { glDeleteVertexArrays(1, &m_VAO); }

void
OpenGLGraphicsPipeline::SetShader(const std::shared_ptr<Shader>& shader) {
  m_shader = std::dynamic_pointer_cast<OpenGLShader>(shader);
  LOG_IF(ERROR, m_shader == nullptr) << "shader is not a opengl shader";
}

void
OpenGLGraphicsPipeline::SetVertexBufferLayout(const Vector<ElementLayout>& vertexBufferLayout,
                                              VertexInputRate rate) {
  m_vertexBufferLayout = vertexBufferLayout;
  m_vertexInputRate = rate;
}

void
OpenGLGraphicsPipeline::SetVertexInputBindingDivisor(
    const Vector<BindingDivisorInfo>& divisorDescription) {
  m_bindingDivisorInfo = divisorDescription;
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
OpenGLGraphicsPipeline::Create() {
  for (const auto& elementInfo : m_vertexBufferLayout) {
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
  if (m_vertexInputRate == VertexInputRate::INSTANCE) {
    for (const auto& [binding, divisor] : m_bindingDivisorInfo) {
      // https://stackoverflow.com/questions/50650457/what-is-the-difference-between-glvertexattribdivisor-and-glvertexbindingdivisor
      glVertexArrayBindingDivisor(m_VAO, binding, divisor);
      glVertexArrayAttribBinding(m_VAO, binding, binding);
    }
  }
}

void
OpenGLGraphicsPipeline::Bind() const {
  glViewport(m_viewPortInfo.x, m_viewPortInfo.y, m_viewPortInfo.width, m_viewPortInfo.height);
  // glScissor(m_scissorInfo.x, m_scissorInfo.y, m_scissorInfo.x, m_scissorInfo.y);

  // set depth test
  if (m_depthStencilInfo.depthTestEnable) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(ConvertToOpenGLDepthCompareFunc(m_depthStencilInfo.depthCompareOp));
  } else {
    glDisable(GL_DEPTH_TEST);
  }

  // set blend
  if (m_blendInfo.logicOpEnable) {
    auto logicOp = ConvertToOpenGLLogicOp(m_blendInfo.logicOp);
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(logicOp);
  } else {
    auto [r, g, b, a] = m_blendInfo.constances;
    glBlendColor(r, g, b, a);
    for (int i = 0; i < m_blendInfo.attachments.size(); i++) {
      const auto& blendAttachment = m_blendInfo.attachments[i];
      if (!blendAttachment.blendEnable) {
        glDisablei(GL_BLEND, i);
        continue;
      }
      glEnablei(GL_BLEND, i);
      auto colorFunc = ConvertToOpenGLBlendEquation(blendAttachment.colorBlendOp);
      auto alphaFunc = ConvertToOpenGLBlendEquation(blendAttachment.alphaBlendOp);
      auto srcColorFactor = ConvertToOpenGLBlendFuctor(blendAttachment.srcColorBlendFactor);
      auto srcAlphaFactor = ConvertToOpenGLBlendFuctor(blendAttachment.srcAlphaBlendFactor);
      auto dstColorFactor = ConvertToOpenGLBlendFuctor(blendAttachment.dstColorBlendFactor);
      auto dstAlphaFactor = ConvertToOpenGLBlendFuctor(blendAttachment.dstAlphaBlendFactor);
      glBlendEquationSeparatei(i, colorFunc, alphaFunc);
      glBlendFuncSeparatei(i, srcColorFactor, dstColorFactor, srcAlphaFactor, dstAlphaFactor);
    }
  }

  m_shader->Use();
  glBindVertexArray(m_VAO);
}

void
OpenGLGraphicsPipeline::UnBind() const {
  glDisable(GL_DEPTH_TEST);
  if (m_blendInfo.logicOpEnable) {
    glDisable(GL_COLOR_LOGIC_OP);
  } else {
    glDisable(GL_BLEND);
  }
}

}  // namespace Marbas
