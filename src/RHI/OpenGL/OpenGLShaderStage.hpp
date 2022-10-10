#pragma once

#include "RHI/Interface/ShaderStage.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

class OpenGLShaderStage : public ShaderStage {
 public:
  explicit OpenGLShaderStage(const ShaderType& shaderType);
  ~OpenGLShaderStage() override;

 public:
  void
  ReadSPIR_V(const FileSystem::path& path, const String& enterPoint) override;

  Vector<uint32_t>
  CompileFromSource(const Path& filePath) override;

  void
  ReadFromSource(const FileSystem::path& path) override;

  [[nodiscard]] GLuint
  GetShaderID() const noexcept {
    return shaderID;
  }

  void
  EnableSpriv(bool isEnable) {
    m_enableSpriv = isEnable;
  }

 private:
  GLuint shaderID;
};

}  // namespace Marbas
