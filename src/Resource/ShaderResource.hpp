#ifndef MARBAS_RESOURCE_SHADER_RESOURCE_HPP
#define MARBAS_RESOURCE_SHADER_RESOURCE_HPP

#include "RHI/RHI.hpp"
#include "Resource/ResourceBase.hpp"

namespace Marbas {

struct ShaderFileInfo {
  ShaderCodeType type;
  Path vertexShaderPath;
  Path fragmentShaderPath;
};

class ShaderResource final : public ResourceBase {
 public:
  explicit ShaderResource(const ShaderFileInfo& shaderFileInfo)
      : ResourceBase(), m_shaderFileInfo(shaderFileInfo) {}

 public:
  void LoadResource(RHIFactory* rhiFactory) override;

  void SetVertexShader(std::unique_ptr<ShaderCode>&& vertexShader) {
    m_vertexShader = std::move(vertexShader);
  }

  void SetFragmentShader(std::unique_ptr<ShaderCode>&& fragmentShader) {
    m_fragmentShader = std::move(fragmentShader);
  }

  [[nodiscard]] Shader* GetShader() const noexcept { return m_shader.get(); }

 private:
  std::unique_ptr<Shader> m_shader;
  std::unique_ptr<ShaderCode> m_vertexShader;
  std::unique_ptr<ShaderCode> m_fragmentShader;

  bool m_isLoad = false;

  const ShaderFileInfo m_shaderFileInfo;
};

}  // namespace Marbas

#endif
