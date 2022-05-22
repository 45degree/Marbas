#pragma once

#include "Common/Common.hpp"
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
  void
  LoadResource(RHIFactory* rhiFactory, std::shared_ptr<ResourceManager>& resourceManager) override;

  void
  SetVertexShader(std::unique_ptr<ShaderStage>&& vertexShader) {
    m_vertexShader = std::move(vertexShader);
  }

  void
  SetFragmentShader(std::unique_ptr<ShaderStage>&& fragmentShader) {
    m_fragmentShader = std::move(fragmentShader);
  }

  [[nodiscard]] std::shared_ptr<Shader>
  GetShader() const noexcept {
    return m_shader;
  }

 private:
  std::shared_ptr<Shader> m_shader;
  std::shared_ptr<ShaderStage> m_vertexShader;
  std::shared_ptr<ShaderStage> m_fragmentShader;

  bool m_isLoad = false;

  const ShaderFileInfo m_shaderFileInfo;
};

}  // namespace Marbas
