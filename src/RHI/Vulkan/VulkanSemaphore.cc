#include <vulkan/vulkan.hpp>

#include "RHI/Interface/Semaphore.hpp"

namespace Marbas {

template <>
void
Semaphore::SetHandler<vk::Semaphore>(const vk::Semaphore &handler) {
  auto vkSemaphore = static_cast<VkSemaphore>(handler);
  m_handler = reinterpret_cast<uintptr_t>(vkSemaphore);
}

template <>
vk::Semaphore
Semaphore::GetHandler<vk::Semaphore>() const {
  auto vkSemaphore = reinterpret_cast<VkSemaphore>(m_handler);
  return static_cast<vk::Semaphore>(vkSemaphore);
}

}  // namespace Marbas
