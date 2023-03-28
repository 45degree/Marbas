#pragma once

#include <list>

#include "Common/Common.hpp"

namespace Marbas {

template <typename Key, typename Value>
class LRUCache final {
 public:
  LRUCache(size_t capacity) : m_capacity(capacity) {}

  void
  insert(const Key& key, const Value& value) {
    if (m_value.find(key) != m_value.end()) {  // find the key in the cache
      m_value[key] = value;
      auto iter = std::find(m_cache.begin(), m_cache.end(), key);
      m_cache.splice(m_cache.begin(), m_cache, iter);
      return;
    }

    // the key is not in cache
    m_value[key] = value;
    m_cache.push_front(key);
    prune();
  }

  const Value&
  at(const Key& key) const {
    // TODO: move the element
    if (!existed(key)) {
      throw std::out_of_range("key is not in the cache");
    }
    return m_value.at(key);
  }

  Value&
  at(const Key& key) {
    if (!existed(key)) {
      throw std::out_of_range("key is not in the cache");
    }
    return m_value[key];
  }

  bool
  existed(const Key& key) const {
    return m_value.find(key) != m_value.end();
  }

  void
  Resize(size_t size) {
    m_capacity = size;
    prune();
  }

  void
  prune() {
    if (m_cache.size() <= m_capacity) {
      return;
    }

    while (m_cache.size() > m_capacity) {
      m_value.erase(m_cache.back());
      m_cache.pop_back();
    }
  }

  void
  clear() {
    m_cache.clear();
    m_value.clear();
  }

  void
  remove(const Key& key) {
    auto iter = std::find(m_cache.begin(), m_cache.end(), key);
    if (iter != m_cache.end()) {
      m_cache.erase(iter);
    }

    if (m_value.find(key) != m_value.end()) {
      m_value.erase(key);
    }
  }

 private:
  size_t m_capacity;
  std::list<Key> m_cache;
  HashMap<Key, Value> m_value;
};

}  // namespace Marbas
