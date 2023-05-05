#pragma once

#include "AssetManager/ModelAsset.hpp"
#include "Core/Scene/GPUDataPipeline/TextureGPUData.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

struct MeshComponent {
  size_t index;
  std::shared_ptr<ModelAsset> m_modelAsset;
};

}  // namespace Marbas
