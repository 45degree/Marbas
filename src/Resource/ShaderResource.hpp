#pragma once

#include "Common/Common.hpp"
#include "RHI/RHI.hpp"
#include "Resource/ResourceBase.hpp"

namespace Marbas {

struct ShaderFileInfo {
  Path vertexShaderPath;
  Path fragmentShaderPath;
};

class ShaderResource final : public ResourceBase {
 public:
  explicit ShaderResource() : ResourceBase() {}

 public:
  void
  LoadResource(RHIFactory* rhiFactory, const ResourceManager* resourceManager) override;

  void
  SetShaderStage(ShaderType type, const Path& path) {
    m_shaderStagePath[type] = path;
  }

  // void
  // SetVertexShader(std::unique_ptr<ShaderStage>&& vertexShader) {
  //   m_vertexShader = std::move(vertexShader);
  // }
  //
  // void
  // SetFragmentShader(std::unique_ptr<ShaderStage>&& fragmentShader) {
  //   m_fragmentShader = std::move(fragmentShader);
  // }

  [[nodiscard]] std::shared_ptr<Shader>
  GetShader() const noexcept {
    return m_shader;
  }

 private:
  std::shared_ptr<Shader> m_shader;
  // std::shared_ptr<ShaderStage> m_vertexShader;
  // std::shared_ptr<ShaderStage> m_fragmentShader;

  std::unordered_map<ShaderType, Path> m_shaderStagePath;
  std::unordered_map<ShaderType, std::shared_ptr<ShaderStage>> m_shaderStages;

  bool m_isLoad = false;

  // const ShaderFileInfo m_shaderFileInfo;
};

}  // namespace Marbas
