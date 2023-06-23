#include "GPUDataManager.hpp"

#include "TextureGPUData.hpp"

namespace Marbas {

void
GPUDataManager::SetUp(RHIFactory* rhiFactory) {
  TextureGPUDataManager::GetInstance()->SetRHI(rhiFactory);
}

void
GPUDataManager::TearDown() {
  TextureGPUDataManager::Destroy();
}

}  // namespace Marbas
