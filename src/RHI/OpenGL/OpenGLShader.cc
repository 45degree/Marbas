#include "RHI/OpenGL/OpenGLShader.hpp"

#include <glog/logging.h>

#include "RHI/OpenGL/OpenGLShaderStage.hpp"
#include "RHI/OpenGL/OpenGLTexture.hpp"

namespace Marbas {

OpenGLShader::OpenGLShader() { programID = glCreateProgram(); }

OpenGLShader::~OpenGLShader() { glDeleteProgram(programID); }

void
OpenGLShader::AddShaderStage(const std::shared_ptr<ShaderStage>& shaderCode) {
  auto openglShader = std::dynamic_pointer_cast<OpenGLShaderStage>(shaderCode);
  if (openglShader == nullptr) {
    LOG(ERROR) << "shader is not a opengl shader";
    throw std::runtime_error("shader can't dynamic cast to OpenglShader");
  }

  auto shaderId = openglShader->GetShaderID();
  glAttachShader(programID, shaderId);
}

void
OpenGLShader::Link() {
  glLinkProgram(programID);
  GLint success = 0;
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(programID, 512, nullptr, infoLog);
    LOG(ERROR) << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << String(infoLog);
  }

  m_isLink = true;
}

void
OpenGLShader::Use() const {
  glUseProgram(programID);
}

}  // namespace Marbas
