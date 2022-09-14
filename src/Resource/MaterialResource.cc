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

  if (m_albedoTexture != nullptr) {
    if (!m_albedoTexture->IsLoad()) {
      m_albedoTexture->LoadResource(rhiFactory, resourceManager);
    }
  }

  if (m_AOTexture != nullptr) {
    if (!m_AOTexture->IsLoad()) {
      m_AOTexture->LoadResource(rhiFactory, resourceManager);
    }
  }

  if (m_normalTexture != nullptr) {
    if (!m_normalTexture->IsLoad()) {
      m_normalTexture->LoadResource(rhiFactory, resourceManager);
    }
  }

  if (m_metallicTexture != nullptr) {
    if (!m_metallicTexture->IsLoad()) {
      m_metallicTexture->LoadResource(rhiFactory, resourceManager);
    }
  }

  if (m_roughnessTexture != nullptr) {
    if (!m_roughnessTexture->IsLoad()) {
      m_roughnessTexture->LoadResource(rhiFactory, resourceManager);
    }
  }

  m_isLoad = true;
}

}  // namespace Marbas
