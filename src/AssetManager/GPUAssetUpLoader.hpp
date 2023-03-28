#pragma once

#include "AssetException.hpp"
#include "AssetManager.hpp"
#include "Singleton.hpp"
#include "Uid.hpp"

namespace Marbas {

template <typename T>
class GPUAssetBase;

template <typename GPUAsset>
class GPUAssetManagerImpl final {
  std::unordered_map<Uid, std::shared_ptr<GPUAsset>> m_assetCache;

 public:
  template <typename... Args>
  void
  Create(Uid uid, Args&&... args) {
    auto gpuAsset = GPUAsset::LoadToGPU(uid, std::forward<Args>(args)...);
    m_assetCache[uid] = gpuAsset;
  }

  template <AssetType AssetType, typename... Args>
  void
  Create(const std::shared_ptr<AssetType>& asset, Args&&... args) {
    auto gpuAsset = GPUAsset::LoadToGPU(asset, std::forward<Args>(args)...);
    m_assetCache[asset->GetUid()] = gpuAsset;
  }

  std::shared_ptr<GPUAsset>
  Get(Uid uid) const {
    if (m_assetCache.find(uid) == m_assetCache.cend()) {
      throw AssetException("can't find gpu asset", uid);
    }
    return m_assetCache.at(uid);
  }

  bool
  Exists(Uid uid) const {
    return m_assetCache.find(uid) != m_assetCache.cend();
  }

  void
  Clear() {
    m_assetCache.clear();
  }

  template <AssetType AssetType, typename... Args>
  void
  Update(const std::shared_ptr<AssetType>& asset, Args&&... args) {
    std::shared_ptr<GPUAssetBase<GPUAsset>> gpuAsset = Get(asset->GetUid());
    gpuAsset->Update(asset, std::forward<Args>(args)...);
  }
};

template <typename GPUAsset>
class GPUAssetBase {
  friend class GPUAssetManagerImpl<GPUAsset>;

 protected:
  template <AssetType CPUAsset, typename... Args>
  void
  Update(const std::shared_ptr<CPUAsset>& asset, Args&&... args) {
    static_cast<GPUAsset*>(this)->Update(asset, std::forward<Args>(args)...);
  }
};

template <typename GPUAsset>
using GPUAssetManager = Singleton<GPUAssetManagerImpl<GPUAsset>>;

template <typename GPUAsset>
using GPUAssetManagerType = std::remove_cvref_t<decltype(*GPUAssetManager<GPUAsset>::GetInstance())>;

}  // namespace Marbas
