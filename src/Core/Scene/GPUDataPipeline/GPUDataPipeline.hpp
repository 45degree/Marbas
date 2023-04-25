#pragma once

#include <async_simple/coro/Lazy.h>
#include <async_simple/coro/SyncAwait.h>

#include <concepts>
#include <entt/entt.hpp>
#include <future>
#include <thread>
#include <unordered_map>

#include "AssetManager/AssetManager.hpp"
#include "AssetManager/ResourceDataCache.hpp"
#include "AssetManager/Singleton.hpp"
#include "AssetManager/Uid.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

template <typename T = void>
using Task = async_simple::coro::Lazy<T>;

class GPUDataPipelineDataBase {
 public:
  void
  SetRHI(RHIFactory* rhiFactory) {
    m_rhiFactory = rhiFactory;
  }

 protected:
  RHIFactory* m_rhiFactory = nullptr;
};

template <typename T>
concept GPUDataPipelineData = requires { std::derived_from<T, GPUDataPipelineDataBase>; };

template <typename Key, GPUDataPipelineData T>
class GPUDataPipelineBase : public ResourceDataCache<Key, T> {
 public:
  void
  SetRHI(RHIFactory* rhiFactory) {
    m_rhiFactory = rhiFactory;
  }

  template <typename... Args>
  void
  Create(Key key, Args&&... args) {
    auto data = std::make_shared<T>();
    data->SetRHI(this->m_rhiFactory);
    data->Load(std::forward<Args>(args)...).start([=, this](auto&&) { this->Insert(key, data); });
  }

  std::shared_ptr<T>
  TryGet(Key entity) {
    return this->Get(entity);
  }

  template <typename... Args>
  void
  Update(Key key, Args&&... args) {
    auto data = TryGet(key);
    if (data == nullptr) return;
    data->Update(std::forward<Args>(args)...).start([](auto&&) {});
  }

  bool
  Existed(Key key) {
    return this->IsInUse(key);
  }

 protected:
  RHIFactory* m_rhiFactory = nullptr;
};

template <GPUDataPipelineData T>
class GPUDataPipelineFromAssetBase final : public GPUDataPipelineBase<Uid, T> {
  using Base = GPUDataPipelineBase<Uid, T>;

 public:
  template <AssetType Asset>
  void
  Create(const Asset& asset) {
    Base::Create(asset.GetUid(), asset);
  }

  template <AssetType Asset>
  std::shared_ptr<T>
  TryGet(const Asset& asset) {
    return Base::TryGet(asset.GetUid());
  }

  template <AssetType Asset>
  void
  Update(const Asset& asset) {
    auto data = TryGet(asset);
    if (data == nullptr) return;
    data->Update(asset).start([](auto&&) {});
  }

  template <AssetType Asset>
  bool
  Existed(const Asset& asset) {
    return Base::Existed(asset.GetUid());
  }
};

template <GPUDataPipelineData T>
using GPUDataPipelineFromAssetManager = Singleton<GPUDataPipelineFromAssetBase<T>>;

template <GPUDataPipelineData T>
using GPUDataPipelineFromEntityManager = Singleton<GPUDataPipelineBase<entt::entity, T>>;

}  // namespace Marbas
