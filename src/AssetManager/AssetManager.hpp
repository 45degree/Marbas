#pragma once

#include <async_simple/coro/Lazy.h>
#include <async_simple/coro/SyncAwait.h>

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <concepts>
#include <fstream>
#include <memory>

#include "AssetException.hpp"
#include "AssetPath.hpp"
#include "AssetRegistry.hpp"
#include "Common/Common.hpp"
#include "ResourceDataCache.hpp"
#include "Singleton.hpp"
#include "Uid.hpp"

namespace Marbas {

template <typename T = void>
using Task = async_simple::coro::Lazy<T>;

template <typename T = void>
using Try = async_simple::Try<T>;

struct AssetBase {
 public:
  void
  SetUid(Uid uid) {
    this->m_uid = uid;
  }

  Uid
  GetUid() const {
    return m_uid;
  }

  template <class Archive>
  void
  serialize(Archive& ar) {
    ar(m_uid);
  }

 protected:
  Uid m_uid;
};

template <typename T>
concept AssetType = requires() { requires std::derived_from<std::remove_cvref_t<T>, AssetBase>; };

/**
 * @brief Assert Manager
 *
 * @tparam Assert Assert class
 */
template <AssetType Asset>
class AssetManagerBase final : public ResourceDataCache<Uid, Asset> {
  using Base = ResourceDataCache<Uid, Asset>;

 public:
  void
  Tick() {
    /**
     * remove all unused assert
     */
    Vector<Uid> needRemovedUid;
    for (auto& [uid, usedAssert] : this->m_usingData) {
      if (usedAssert.expired()) {
        needRemovedUid.push_back(uid);
      }
    }

    for (auto& uid : needRemovedUid) {
      this->m_usingData.erase(uid);
    }
  }

  /**
   * @brief get the assert by path, if the assert had beed created, it will be load from disk,
   *        if the assert is not stored in this disk, it will throw an exception.
   *
   * @tparam Assert assert type
   * @param path
   * @return
   */
  std::shared_ptr<Asset>
  Get(const AssetPath& path) {
    auto* registry = AssetRegistry::GetInstance();
    auto uid = registry->CreateOrFindAssertUid(path);
    try {
      auto asset = Get(uid);
      return asset;
    } catch (AssetException& exception) {
      throw AssetException("can't find resource in the .import dir, maybe you not create it", path);
    }
  }

  std::shared_ptr<Asset>
  Get(Uid uid) {
    auto asset = Base::Get(uid);
    if (asset != nullptr) return asset;

    auto* registry = AssetRegistry::GetInstance();
    auto assertPath = registry->GetAssertAbsolutePath(uid);
    if (!std::filesystem::exists(assertPath)) {
      throw AssetException("can't find resource in the .import dir, maybe you not create it", uid);
    }

    std::ifstream file(assertPath, std::ios::binary | std::ios::in);
    cereal::BinaryInputArchive ar(file);

    ar(asset);
    Base::Insert(uid, asset);
    return asset;
  }

  Task<std::shared_ptr<Asset>>
  GetAsync(const AssetPath& path) {
    auto* registry = AssetRegistry::GetInstance();
    auto uid = registry->CreateOrFindAssertUid(path);
    try {
      auto asset = co_await GetAsync(uid);
      co_return asset;
    } catch (AssetException& exception) {
      throw AssetException("can't find resource in the .import dir, maybe you not create it", path);
    }
  }

  Task<std::shared_ptr<Asset>>
  GetAsync(const Uid& uid) {
    auto asset = Base::Get(uid);
    if (asset != nullptr) co_return asset;

    auto* registry = AssetRegistry::GetInstance();
    auto assertPath = registry->GetAssertAbsolutePath(uid);
    if (!std::filesystem::exists(assertPath)) {
      throw AssetException("can't find resource in the .import dir, maybe you not create it", uid);
    }

    std::ifstream file(assertPath, std::ios::binary | std::ios::in);
    cereal::BinaryInputArchive ar(file);

    ar(asset);
    Base::Insert(uid, asset);
    co_return asset;
  }

  /**
   * @brief check if a asset is exists in the disk(.import dir of the project dir)
   *
   * @param path
   * @return
   */
  bool
  Existed(const AssetPath& path) {
    auto* registry = AssetRegistry::GetInstance();
    if (!registry->Existed(path)) return false;

    auto uid = registry->CreateOrFindAssertUid(path);
    auto assertPath = registry->GetAssertAbsolutePath(uid);
    return std::filesystem::exists(assertPath);
  }

  /**
   * @brief create the assert, and store the assert to the disk, if the disk existed a resouce,
   *        it will throw an exception;
   *
   * @param path asset path
   */
  template <typename... Args>
  Uid
  Create(const AssetPath& path, Args&&... args) {
    auto* registry = AssetRegistry::GetInstance();
    auto uid = registry->CreateOrFindAssertUid(path);
    auto assertPath = registry->GetAssertAbsolutePath(uid);

    if (std::filesystem::exists(assertPath)) {
      throw AssetException("the assert existed in the disk", path);
    }

    Asset::Load(path, std::forward<Args>(args)...).start([&](Try<std::shared_ptr<Asset>> result) {
      if (result.hasError()) {
        return;
      }
      auto asset = result.value();
      asset->SetUid(uid);

      std::ofstream file(assertPath, std::ios::binary | std::ios::out);
      cereal::BinaryOutputArchive ar(file);
      ar(asset);

      Base::Insert(uid, asset);
    });

    return uid;
  }

  template <typename... Args>
  Task<std::shared_ptr<Asset>>
  CreateAsync(const AssetPath& path, Args&&... args) {
    auto* registry = AssetRegistry::GetInstance();
    auto uid = registry->CreateOrFindAssertUid(path);
    auto assertPath = registry->GetAssertAbsolutePath(uid);

    if (std::filesystem::exists(assertPath)) {
      throw AssetException("the assert existed in the disk", path);
    }

    auto asset = co_await Asset::Load(path, std::forward<Args>(args)...);
    asset->SetUid(uid);

    std::ofstream file(assertPath, std::ios::binary | std::ios::out);
    cereal::BinaryOutputArchive ar(file);
    ar(asset);

    Base::Insert(uid, asset);
    co_return asset;
  }

  /**
   * @brief delete the assert by path
   *
   * delete the binary from import directory and registry
   *
   * @param path
   */
  void
  Delete(const AssetPath& path) {
    auto* registry = AssetRegistry::GetInstance();
    if (!registry->Existed(path)) return;

    auto uid = registry->CreateOrFindAssertUid(path);
    auto assertPath = registry->GetAssertAbsolutePath(uid);
    std::filesystem::remove(assertPath);
  }

  /**
   * @brief save all resource in used
   */
  void
  Save() {
    for (auto&& [uid, usedAssert] : this->m_usingData) {
      if (usedAssert.expired()) {
        continue;
      }
      auto* registry = AssetRegistry::GetInstance();
      auto assertPath = registry->GetAssertAbsolutePath(uid);
      std::ofstream file(assertPath, std::ios::binary | std::ios::out);
      cereal::BinaryOutputArchive ar(file);
      ar(usedAssert);
    }
  }
};

template <AssetType Assert>
using AssetManager = Singleton<AssetManagerBase<Assert>>;

template <AssetType Assert>
using AssetManagerType = std::remove_cvref_t<decltype(*AssetManager<Assert>::GetInstance())>;

}  // namespace Marbas
