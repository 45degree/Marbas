#pragma once

#include "Common/Common.hpp"

namespace Marbas {

enum class ShaderType {
  VERTEX_SHADER,
  FRAGMENT_SHADER,
  GEOMETRY_SHADER,
};

class ShaderStage {
 public:
  explicit ShaderStage(const ShaderType& shaderType) : shaderType(shaderType){};
  virtual ~ShaderStage() = default;

 public:
  /**
   * @brief read spri-v file
   *
   * SPIR-V is similar to GLSL, but it has some differences. Two differences are particularly
   * relevant.
   *
   * 1. A single SPIR-V file can have function entry-points for multiple shader stages, even of
   *    different types.
   * 2. SPIR-V has the concept of "specialization constants": parameters
   *    which the user can provide before the SPIR-V is compiled into its final form.
   *
   * @see https://www.khronos.org/opengl/wiki/SPIR-V
   *
   * @param path the path of spri-v file
   * @param enterPoint the enterPoint of spri-v file
   *
   * @note we assume the spri-v file don't have specialization constants.
   */
  virtual void
  ReadSPIR_V(const Path& path, const String& enterPoint) = 0;

  virtual Vector<uint32_t>
  CompileFromSource(const Path& filePath) = 0;

  virtual void
  ReadFromSource(const Path& path) {}

  [[nodiscard]] ShaderType
  GetShaderType() const noexcept {
    return shaderType;
  };

  bool
  IsEnableSpriv() const {
    return m_enableSpriv;
  }

 protected:
  ShaderType shaderType;
  bool m_enableSpriv = true;
};

}  // namespace Marbas
