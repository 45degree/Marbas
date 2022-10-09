#pragma once

#include <cstdint>

namespace Marbas {

class Semaphore {
 public:
  Semaphore() = default;

 public:
  template <typename T>
  void
  SetHandler(const T& handler);

  template <typename T>
  T
  GetHandler() const;

 private:
  uintptr_t m_handler = 0;
};

}  // namespace Marbas
