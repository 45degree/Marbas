#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace Marbas {

class VulkanTexture {
 public:
  explicit VulkanTexture(uintptr_t handler) : m_handler(handler) {}

  template <typename T>
  T
  GetImageHandle() const {
    return static_cast<T>(m_handler);
  }

  template <>
  vk::Image
  GetImageHandle<vk::Image>() const {
    return static_cast<vk::Image>(reinterpret_cast<VkImage>(m_handler));
  }

 private:
  uintptr_t m_handler;
};

}  // namespace Marbas
