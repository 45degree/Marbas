#pragma once

#include <vulkan/vulkan.hpp>

#include "RHI/Interface/RenderPass.hpp"

namespace Marbas {

class VulkanRenderPass final : public RenderPass {
 public:
  explicit VulkanRenderPass(const RenderPassCreateInfo& createInfo, const vk::Device& m_device);
  // TODO:
  virtual ~VulkanRenderPass() = default;

 private:
  vk::RenderPass m_renderPass;
  vk::Device m_device;
};

}  // namespace Marbas
