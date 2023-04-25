#include "GPUDataManager.hpp"

#include "LightGPUData.hpp"
#include "ModelGPUData.hpp"
#include "TextureGPUData.hpp"

namespace Marbas {

void
GPUDataManager::SetUp(RHIFactory* rhiFactory) {
  TextureGPUDataManager::GetInstance()->SetRHI(rhiFactory);
  ModelGPUDataManager::GetInstance()->SetRHI(rhiFactory);
  LightGPUDataManager::GetInstance()->SetRHI(rhiFactory);

  LightGPUData::Initialize(rhiFactory);
}

void
GPUDataManager::TearDown() {
  TextureGPUDataManager::Destroy();
  ModelGPUDataManager::Destroy();
  LightGPUDataManager::Destroy();
}

}  // namespace Marbas
