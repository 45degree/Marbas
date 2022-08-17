#include "Resource/MaterialResource.hpp"

#include "Resource/ResourceManager.hpp"

namespace Marbas {

void
MaterialResource::LoadResource(RHIFactory* rhiFactory, const ResourceManager* resourceManager) {
  if (m_shaderResource != nullptr) {
    if (!m_shaderResource->IsLoad()) {
      m_shaderResource->LoadResource(rhiFactory, resourceManager);
    }
  }

  if (m_diffuseTexture != nullptr) {
    if (!m_diffuseTexture->IsLoad()) {
      m_diffuseTexture->LoadResource(rhiFactory, resourceManager);
    }
  }

  if (m_ambientTexture != nullptr) {
    if (!m_ambientTexture->IsLoad()) {
      m_ambientTexture->LoadResource(rhiFactory, resourceManager);
    }
  }

  m_isLoad = true;
}

}  // namespace Marbas
