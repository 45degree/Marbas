#include "RHI/OpenGL/OpenGLShaderStage.hpp"

#include <glog/logging.h>

#include <fstream>
#include <iostream>
#include <shaderc/shaderc.hpp>

#include "shaderc/shaderc.h"

namespace Marbas {

static GLenum
ConvertToOpenGLShaderType(const ShaderType& type) noexcept {
  switch (type) {
    case ShaderType::VERTEX_SHADER:
      return GL_VERTEX_SHADER;
    case ShaderType::FRAGMENT_SHADER:
      return GL_FRAGMENT_SHADER;
  }
}

static void
GetCompileOption(shaderc::CompileOptions& options) {
  options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);

  options.AddMacroDefinition("OPENGL");
#ifdef NDEBUG
  options.SetOptimizationLevel(shaderc_optimization_level_size);
#endif
}

void
OpenGLShaderStage::ReadSPIR_V(const FileSystem::path& path, const String& enterPoint) {
  std::ifstream file;
  file.open(path, std::ios_base::in | std::ios_base::binary);
  if (!file.is_open()) return;

  // read the spri-v content
  Vector<char> content;
  try {
    file.seekg(0, std::ios::beg);
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    content.resize(size);
    file.seekg(0, std::ios::beg);
    file.read(content.data(), size);
  } catch (const std::exception& e) {
    file.close();
    throw std::exception(e);
  }

  auto contentSize = static_cast<GLsizei>(content.size());
  glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, content.data(), contentSize);
  glSpecializeShader(shaderID, enterPoint.c_str(), 0, nullptr, nullptr);

  // Specialization is equivalent to compilation.
  GLint isCompiled = 0;
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> infoLog(maxLength);
    glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

    LOG(INFO) << infoLog.data();

    // We don't need the shader anymore.
    glDeleteShader(shaderID);

    // Use the infoLog as you see fit.

    // In this simple program, we'll just leave
    return;
  }
}

Vector<uint32_t>
OpenGLShaderStage::CompileFromSource(const Path& path) {
  // read content
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  // read source file
  std::string content;
  try {
    file.open(path);
    file.seekg(0, std::ios::beg);

    std::stringstream stringStream;
    stringStream << file.rdbuf();
    file.close();

    content = stringStream.str();
  } catch (const std::exception& e) {
    LOG(ERROR) << FORMAT("can't read the file {}, and cause an exception: {}", path, e.what());

    if (file.is_open()) {
      file.close();
    }

    throw e;
  }

  shaderc_shader_kind kind;
  switch (shaderType) {
    case ShaderType::VERTEX_SHADER:
      kind = shaderc_glsl_vertex_shader;
      break;
    case ShaderType::FRAGMENT_SHADER:
      kind = shaderc_glsl_fragment_shader;
      break;
  }

  // compile shader
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;
  GetCompileOption(options);

  auto module = compiler.CompileGlslToSpv(content, kind, path.string().c_str(), options);

  if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
    LOG(WARNING) << FORMAT("can't compile the shader: {}", module.GetErrorMessage());
    return {};
  }

  module.GetNumWarnings();

  return {module.cbegin(), module.cend()};
}

void
OpenGLShaderStage::ReadFromSource(const FileSystem::path& path) {
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  // read source file
  std::string content;
  try {
    file.open(path);
    file.seekg(0, std::ios::beg);

    std::stringstream stringStream;
    stringStream << file.rdbuf();
    file.close();

    content = stringStream.str();
  } catch (const std::exception& e) {
    LOG(ERROR) << FORMAT("can't read the file {}, and cause an exception: {}", path, e.what());

    if (file.is_open()) {
      file.close();
    }

    throw e;
  }

  /**
   * preprocessing glsl
   */
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;
  GetCompileOption(options);

  shaderc_shader_kind kind;
  switch (shaderType) {
    case ShaderType::VERTEX_SHADER:
      kind = shaderc_glsl_vertex_shader;
      break;
    case ShaderType::FRAGMENT_SHADER:
      kind = shaderc_glsl_fragment_shader;
      break;
  }

  shaderc::PreprocessedSourceCompilationResult result =
      compiler.PreprocessGlsl(content, kind, "", options);

  if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
    LOG(ERROR) << FORMAT("can't preprocessing the glsl: {}", result.GetErrorMessage());
    return;
  }
  std::string shaderSource = {result.cbegin(), result.cend()};

  /**
   * compile the shader
   */
  const char* shaderSourcePtr = shaderSource.c_str();
  glShaderSource(shaderID, 1, &shaderSourcePtr, nullptr);
  glCompileShader(shaderID);

  GLint success = 0;
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar infoLog[1024];
    glGetShaderInfoLog(shaderID, 1024, nullptr, infoLog);
    LOG(ERROR) << "failed to compile shader, info: \n" << String(infoLog);
  } else {
    LOG(INFO) << "compiled shader";
  }
}

OpenGLShaderStage::OpenGLShaderStage(const ShaderType& shaderType) : ShaderStage(shaderType) {
  GLenum openglShaderType = ConvertToOpenGLShaderType(shaderType);
  shaderID = glCreateShader(openglShaderType);
}

OpenGLShaderStage::~OpenGLShaderStage() { glDeleteShader(shaderID); }

}  // namespace Marbas
