#include "Resource/ShaderResource.hpp"

#include <glog/logging.h>

namespace Marbas {

void
ShaderResource::LoadResource(RHIFactory* rhiFactory, std::shared_ptr<ResourceManager>&) {
  if (m_isLoad) return;

  m_vertexShader = rhiFactory->CreateShaderStage(m_shaderFileInfo.vertexShaderPath,
                                                 m_shaderFileInfo.type, ShaderType::VERTEX_SHADER);

  m_fragmentShader = rhiFactory->CreateShaderStage(
      m_shaderFileInfo.fragmentShaderPath, m_shaderFileInfo.type, ShaderType::FRAGMENT_SHADER);

  m_shader = rhiFactory->CreateShader();

  m_shader->AddShaderStage(m_vertexShader);
  m_shader->AddShaderStage(m_fragmentShader);
  m_shader->Link();
  m_isLoad = true;
}

}  // namespace Marbas
