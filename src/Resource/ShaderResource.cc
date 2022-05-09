#include "Resource/ShaderResource.hpp"

#include <glog/logging.h>

namespace Marbas {

Shader* ShaderResource::LoadShader() {
  if (m_isLoad) return m_shader.get();

  LOG(INFO) << FORMAT("load shader resource, uid is {}", m_id);

  if (m_shader == nullptr) return nullptr;
  if (m_vertexShader == nullptr || m_fragmentShader == nullptr) return nullptr;
  if (m_isLink) return m_shader.get();

  m_shader->AddShaderCode(m_vertexShader.get());
  m_shader->AddShaderCode(m_fragmentShader.get());
  m_shader->Link();
  m_isLink = true;
  m_isLoad = true;

  return m_shader.get();
}

}  // namespace Marbas
