#pragma once

#include "AssetManager/ModelAsset.hpp"

namespace Marbas {

struct MeshComponent {
  size_t index;
  std::shared_ptr<ModelAsset> m_modelAsset;
};

}  // namespace Marbas
