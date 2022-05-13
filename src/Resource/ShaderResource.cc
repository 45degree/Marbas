#include "Resource/ShaderResource.hpp"

#include <glog/logging.h>

namespace Marbas {

void ShaderResource::LoadResource(RHIFactory* rhiFactory) {
  if (m_isLoad) return;

  LOG(INFO) << FORMAT("load shader resource, uid is {}", m_id);

  m_vertexShader = rhiFactory->CreateShaderCode(m_shaderFileInfo.vertexShaderPath,
                                                m_shaderFileInfo.type, ShaderType::VERTEX_SHADER);

  m_fragmentShader = rhiFactory->CreateShaderCode(
      m_shaderFileInfo.fragmentShaderPath, m_shaderFileInfo.type, ShaderType::FRAGMENT_SHADER);

  m_shader = rhiFactory->CreateShader();

  m_shader->AddShaderCode(m_vertexShader.get());
  m_shader->AddShaderCode(m_fragmentShader.get());
  m_shader->Link();
  m_isLoad = true;
}

}  // namespace Marbas
