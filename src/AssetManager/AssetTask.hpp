#pragma once

#include <memory>

namespace Marbas {

template <typename T>
class AssetTask final {
 public:
  AssetTask() = default;
  AssetTask(const AssetTask& obj) : m_assert(obj.m_assert) {}
  AssetTask(AssetTask&& obj) : m_assert(std::move(obj.m_assert)) {}
  AssetTask(const std::shared_ptr<T>& assert) : m_assert(assert) {}

  AssetTask&
  operator=(const AssetTask& obj) {
    m_assert = obj.m_assert;
    return *this;
  }

 public:
  std::shared_ptr<T>
  GetValue() const {
    return m_assert;
  }

 private:
  std::shared_ptr<T> m_assert = nullptr;
};

}  // namespace Marbas
