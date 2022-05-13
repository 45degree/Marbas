#include "Resource/MaterialResource.hpp"

#include "Resource/ResourceManager.hpp"

namespace Marbas {

void MaterialResource::LoadResource(RHIFactory* rhiFactory) {
  LOG(INFO) << FORMAT("load material resource, uid is {}", m_id);

  if (m_shaderResource != nullptr) {
    if (!m_shaderResource->IsLoad()) {
      m_shaderResource->LoadResource(rhiFactory);
    }
  } else {
    LOG(ERROR) << FORMAT("material resource {} must have a shader, but it's null", m_id);
  }

  if (m_diffuseTexture != nullptr) {
    if (!m_diffuseTexture->IsLoad()) {
      m_diffuseTexture->LoadResource(rhiFactory);
    }
  } else {
    LOG(WARNING) << FORMAT("material resource {} don't have diffuse texture", m_id);
  }

  if (m_ambientTexture != nullptr) {
    if (!m_ambientTexture->IsLoad()) {
      m_ambientTexture->LoadResource(rhiFactory);
    }
  } else {
    LOG(WARNING) << FORMAT("material resource {} don't have ambient texture", m_id);
  }

  if (m_material == nullptr) {
    m_material = std::make_unique<Material>();
  }

  m_material->SetAmbientTexture(m_ambientTexture->GetTexture());
  m_material->SetDiffuseTexture(m_diffuseTexture->GetTexture());
  m_material->SetShader(m_shaderResource->GetShader());

  m_isLoad = true;
}

void CubeMapResource::LoadResource(RHIFactory* rhiFactory) {
  LOG(INFO) << FORMAT("load cubemap material, uid is {}", m_id);

  if(m_cubeMapTexture == nullptr) {
    m_cubeMapTexture = rhiFactory->CreateTextureCubeMap(m_createInfo);
  }

  m_isLoad = true;
}

}  // namespace Marbas
