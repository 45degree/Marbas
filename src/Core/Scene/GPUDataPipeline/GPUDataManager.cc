#include "GPUDataManager.hpp"

#include "LightGPUData.hpp"
#include "MeshGPUData.hpp"
#include "TextureGPUData.hpp"

namespace Marbas {

void
GPUDataManager::SetUp(RHIFactory* rhiFactory) {
  TextureGPUDataManager::GetInstance()->SetRHI(rhiFactory);
  MeshGPUDataManager::GetInstance()->SetRHI(rhiFactory);
  LightGPUDataManager::GetInstance()->SetRHI(rhiFactory);

  // TODO: can remove it?
  MeshGPUDataManager::GetInstance()->ResizeCache(1000);
  LightGPUData::Initialize(rhiFactory);
}

void
GPUDataManager::TearDown() {
  TextureGPUDataManager::Destroy();
  MeshGPUDataManager::Destroy();
  LightGPUDataManager::Destroy();
}

}  // namespace Marbas
