#pragma once

#include <memory>
#include <vector>

#include "LRUCache.hpp"

namespace Marbas {

template <typename Key, typename T>
class ResourceDataCache {
 public:
  void
  RemoveAllUsedData() {
    std::vector<Key> needRemovedUid;
    for (auto& [key, usedAssert] : m_usingData) {
      if (usedAssert.expired()) {
        needRemovedUid.push_back(key);
      }
    }

    for (auto& key : needRemovedUid) {
      m_usingData.erase(key);
    }
  }

  void
  Insert(const Key& key, std::shared_ptr<T> data) {
    m_cache.insert(key, data);
    m_usingData[key] = data;
  }

  std::shared_ptr<T>
  Get(const Key& key) {
    if (m_usingData.find(key) == m_usingData.end()) return nullptr;
    if (m_usingData.at(key).expired()) {
      m_usingData.erase(key);
      return nullptr;
    }

    if (!m_cache.existed(key)) {
      auto assert = m_usingData.at(key).lock();
      m_cache.insert(key, assert);
    }
    return m_cache.at(key);
  }

  void
  RemoveFromCache(const Key& key) {
    m_cache.remove(key);
  }

  void
  ResizeCache(size_t size) {
    m_cache.Resize(size);
  }

  void
  ClearCache() {
    m_cache.clear();
  }

  void
  ClearAll() {
    ClearCache();
    m_cache.clear();
  }

  bool
  IsInCache(Key key) {
    return m_cache.existed(key);
  }

  bool
  IsInUse(Key key) {
    return !m_usingData[key].expired();
  }

 protected:
  LRUCache<Key, std::shared_ptr<T>> m_cache{200};
  HashMap<Key, std::weak_ptr<T>> m_usingData;
};

}  // namespace Marbas
