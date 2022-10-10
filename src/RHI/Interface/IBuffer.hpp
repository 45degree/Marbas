#pragma once

#include <cstdint>

namespace Marbas {

class IBuffer {
 public:
  virtual void
  SetData(const void* data, uint32_t size, uint32_t offset) = 0;
};

}  // namespace Marbas
