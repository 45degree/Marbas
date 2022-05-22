#pragma once

#include "RHI/Interface/IndexBuffer.hpp"
#include "RHI/Interface/ShaderCode.hpp"
#include "RHI/Interface/Texture.hpp"
#include "RHI/Interface/VertexBuffer.hpp"

namespace Marbas {

class Shader {
 public:
  Shader() = default;
  virtual ~Shader() = default;

 public:
  virtual void
  AddShaderStage(const std::shared_ptr<ShaderStage>& shaderCode) = 0;

  virtual void
  Link() = 0;
};

}  // namespace Marbas
