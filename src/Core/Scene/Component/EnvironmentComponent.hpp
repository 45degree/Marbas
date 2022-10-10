#pragma once

#include "Common/Common.hpp"
#include "Resource/TextureResource.hpp"

namespace Marbas {

struct EnvironmentComponent_Impl {
  std::shared_ptr<Texture2DResource> m_textureResource;
};

struct EnvironmentComponent {
  Path hdrImagePath;
  std::shared_ptr<EnvironmentComponent_Impl> implData = nullptr;
};

}  // namespace Marbas
