#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace Marbas {

class VulkanTexture {
 public:
  explicit VulkanTexture(uintptr_t handler) : m_handler(handler) {}

  template <typename T>
  T
  GetImageHandle() const;

 private:
  uintptr_t m_handler;
};

}  // namespace Marbas
