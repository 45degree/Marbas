#include "RHI/Vulkan/VulkanRenderPass.hpp"

namespace Marbas {

VulkanRenderPass::VulkanRenderPass(const RenderPassCreateInfo& createInfo, const vk::Device& device)
    : RenderPass(createInfo), m_device(device) {
  vk::RenderPassCreateInfo vkRenderPassCreateInfo;
  // vkRenderPassCreateInfo.setSubpassCount();
}

}  // namespace Marbas
