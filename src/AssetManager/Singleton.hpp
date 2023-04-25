#pragma once

#include <memory>
#include <mutex>
#include <typeindex>
#include <typeinfo>

namespace Marbas {

template <typename T>
class Singleton {
  static std::once_flag static_flag;
  static std::unique_ptr<T> m_instance;

 public:
  // Get the single instance
  static T*
  GetInstance() {
    std::call_once(static_flag, []() { m_instance = std::make_unique<T>(); });
    return m_instance.get();
  }

  static void
  Destroy() {
    m_instance = nullptr;
  }
};

template <typename T>
std::unique_ptr<T> Singleton<T>::m_instance = nullptr;

template <typename T>
std::once_flag Singleton<T>::static_flag;

}  // namespace Marbas
