#pragma once

#include "RHI/Interface/ShaderCode.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

class OpenGLShaderStage : public ShaderStage {
 public:
  explicit OpenGLShaderStage(const ShaderType& shaderType);
  ~OpenGLShaderStage() override;

 public:
  void ReadSPIR_V(const FileSystem::path& path, const String& enterPoint) override;

  void ReadFromSource(const FileSystem::path& path);

  [[nodiscard]] GLuint GetShaderID() const noexcept { return shaderID; }

 private:
  GLuint shaderID;
};

}  // namespace Marbas
