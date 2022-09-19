#include "Core/Scene/System/LoadImpl.hpp"

namespace Marbas {

template <>
void
LoadComponentImpl<EnvironmentComponent>(EnvironmentComponent& component,
                                        const ResourceManager* resourceMgr) {
  auto texture2DResourceContainer = resourceMgr->GetTexture2DResourceContainer();
  auto resource = texture2DResourceContainer->CreateResource(component.hdrImagePath, 1, true);

  component.implData = std::make_shared<EnvironmentComponent_Impl>();
  component.implData->m_textureResource = std::move(resource);

  return;
}

}  // namespace Marbas
