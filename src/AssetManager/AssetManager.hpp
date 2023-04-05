#pragma once

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <concepts>
#include <fstream>
#include <memory>

#include "AssetException.hpp"
#include "AssetPath.hpp"
#include "AssetRegistry.hpp"
#include "Common/Common.hpp"
#include "LRUCache.hpp"
#include "Singleton.hpp"
#include "Uid.hpp"

namespace Marbas {

struct AssetBase {
 public:
  void
  SetUid(Uid uid) {
    this->m_uid = uid;
  }

  Uid
  GetUid() {
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
concept AssetType = requires() { requires std::derived_from<T, AssetBase>; };

/**
 * @brief Assert Manager
 *
 * @tparam Assert Assert class
 */
template <AssetType Asset>
class AssetManagerImpl final {
  /**
   * @brief the assert cache, all assert loaded or create by this class will store a copy in this cache;
   */
  LRUCache<Uid, std::shared_ptr<Asset>> m_assertCache{200};

  /**
   * @brief all the assert loaded or create by this class will store in it, it cantained the assert that has beed
   *        removed from cache but still used in the frame.
   */
  HashMap<Uid, std::weak_ptr<Asset>> m_usedAssert;

 public:
  void
  Tick() {
    /**
     * remove all unused assert
     */
    Vector<Uid> needRemovedUid;
    for (auto& [uid, usedAssert] : m_usedAssert) {
      if (usedAssert.expired()) {
        needRemovedUid.push_back(uid);
      }
    }

    for (auto& uid : needRemovedUid) {
      m_usedAssert.erase(uid);
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
    auto* registry = AssetRegistry::GetInstance();
    if (m_usedAssert.find(uid) != m_usedAssert.end()) {
      if (!m_usedAssert.at(uid).expired()) {
        if (!m_assertCache.existed(uid)) {
          auto assert = m_usedAssert.at(uid).lock();
          m_assertCache.insert(uid, assert);
        }
        return m_assertCache.at(uid);
      }
      m_usedAssert.erase(uid);
    }

    auto assertPath = registry->GetAssertAbsolutePath(uid);
    if (!std::filesystem::exists(assertPath)) {
      throw AssetException("can't find resource in the .import dir, maybe you not create it", uid);
    }

    std::ifstream file(assertPath, std::ios::binary | std::ios::in);
    cereal::BinaryInputArchive ar(file);

    std::shared_ptr<Asset> assert;
    ar(assert);
    m_assertCache.insert(uid, assert);
    m_usedAssert.insert({uid, assert});
    return assert;
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
  std::shared_ptr<Asset>
  Create(const AssetPath& path, Args&&... args) {
    auto* registry = AssetRegistry::GetInstance();
    auto uid = registry->CreateOrFindAssertUid(path);
    auto assertPath = registry->GetAssertAbsolutePath(uid);

    if (std::filesystem::exists(assertPath)) {
      throw AssetException("the assert existed in the disk", path);
    }

    auto assert = Asset::Load(path.GetAbsolutePath(), std::forward<Args>(args)...);
    assert->SetUid(uid);

    std::ofstream file(assertPath, std::ios::binary | std::ios::out);
    cereal::BinaryOutputArchive ar(file);
    ar(assert);

    m_assertCache.insert(uid, assert);
    m_usedAssert[uid] = assert;

    return assert;
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
    for (auto&& [uid, usedAssert] : m_usedAssert) {
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

  /**
   * @brief insert a asset to the manager
   */
  void
  Insert(const std::shared_ptr<Asset>& asset) {
    m_assertCache.insert(asset->uid, asset);
    m_usedAssert[asset->uid] = asset;
  }

  void
  RemoveFromCache(Uid uid) {
    m_assertCache.remove(uid);
  }

  void
  ResizeCache(size_t size) {
    m_assertCache.Resize(size);
  }

  void
  ClearCache() {
    m_assertCache.clear();
  }

  void
  ClearAll() {
    ClearCache();
    m_usedAssert.clear();
  }

  bool
  IsInCache(Uid uid) {
    return m_assertCache.existed(uid);
  }

  bool
  IsInUse(Uid uid) {
    return !m_usedAssert[uid].expired();
  }
};

template <AssetType Assert>
using AssetManager = Singleton<AssetManagerImpl<Assert>>;

template <AssetType Assert>
using AssetManagerType = std::remove_cvref_t<decltype(*AssetManager<Assert>::GetInstance())>;

}  // namespace Marbas
