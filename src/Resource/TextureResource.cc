#include "Resource/TextureResource.hpp"

#include "Resource/ResourceManager.hpp"

namespace Marbas {

void
Texture2DResource::LoadResource(RHIFactory* rhiFactory, const ResourceManager* resourceManager) {
  if (m_isLoad) return;

  Path path = m_path;
  if (m_path.is_relative()) {
    path = resourceManager->GetWorkspace() / m_path;
  }

  m_texture = rhiFactory->CreateTexture2D(path, m_levels, m_isHDR);

  m_isLoad = true;
}

}  // namespace Marbas
