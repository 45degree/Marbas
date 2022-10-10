#include "RHI/Vulkan/VulkanTexture.hpp"

#include <vulkan/vulkan.hpp>

namespace Marbas {

template <>
vk::Image
VulkanTexture::GetImageHandle<vk::Image>() const {
  return static_cast<vk::Image>(reinterpret_cast<VkImage>(m_handler));
}

}  // namespace Marbas
