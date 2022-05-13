#include "RHI/OpenGL/OpenGLShader.hpp"

#include <glog/logging.h>

#include "RHI/OpenGL/OpenGLShaderCode.hpp"
#include "RHI/OpenGL/OpenGLTexture.hpp"

namespace Marbas {

OpenGLShader::OpenGLShader() { programID = glCreateProgram(); }

OpenGLShader::~OpenGLShader() { glDeleteProgram(programID); }

void OpenGLShader::AddShaderCode(const ShaderCode* shaderCode) {
  auto openglShader = dynamic_cast<const OpenGLShaderCode*>(shaderCode);
  if (openglShader == nullptr) {
    LOG(ERROR) << "shader is not a opengl shader";
    throw std::runtime_error("shader can't dynamic cast to OpenglShader");
  }
  auto shaderId = openglShader->GetShaderID();
  glAttachShader(programID, shaderId);
}

void OpenGLShader::Link() {
  glLinkProgram(programID);
  GLint success = 0;
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(programID, 512, nullptr, infoLog);
    LOG(ERROR) << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
  }

  m_isLink = true;
}

void OpenGLShader::AddUniformDataBlock(uint32_t bindingPoint, const void* data, uint32_t size) {
  if (m_uniformDataBlocks.find(bindingPoint) == m_uniformDataBlocks.end()) {
    m_uniformDataBlocks[bindingPoint] = std::make_unique<OpenGLUniformBuffer>(size, bindingPoint);
  }

  auto* uniformBlock = m_uniformDataBlocks.at(bindingPoint).get();
  uniformBlock->SetData(data, size, 0);
}

void OpenGLShader::Use() const {
  glUseProgram(programID);
  for (const auto& [bindingPoint, buffer] : m_uniformDataBlocks) {
    buffer->Bind();
  }
}

void OpenGLShader::UnBind() const {
  for (const auto& [bindingPoint, buffer] : m_uniformDataBlocks) {
    buffer->UnBind();
  }
}

}  // namespace Marbas
