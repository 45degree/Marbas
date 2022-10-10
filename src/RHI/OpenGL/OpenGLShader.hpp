#pragma once

#include "RHI/Interface/IndexBuffer.hpp"
#include "RHI/Interface/Shader.hpp"
#include "RHI/OpenGL/OpenGLTexture.hpp"
#include "RHI/OpenGL/OpenGLUniformBuffer.hpp"

namespace Marbas {

class OpenGLShader final : public Shader {
 public:
  OpenGLShader();
  ~OpenGLShader() override;

 public:
  void AddShaderStage(const std::shared_ptr<ShaderStage>& shaderCode) override;

  void Link() override;

  void Use() const;

 private:
  GLuint programID;

  bool m_isLink = false;
  std::unordered_map<uint32_t, std::unique_ptr<OpenGLUniformBuffer>> m_uniformDataBlocks;
};

}  // namespace Marbas
