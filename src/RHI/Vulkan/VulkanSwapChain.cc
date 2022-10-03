#include "RHI/Vulkan/VulkanSwapChain.hpp"

#include "glog/logging.h"

namespace Marbas {

VulkanSwapChain::VulkanSwapChain(const VulkanSwapChainCreateInfo& createInfo)
    : SwapChain(800, 600),
      m_device(createInfo.device),
      m_physicalDevice(createInfo.physicalDevice),
      m_surface(createInfo.surface),
      m_glfwWindow(createInfo.glfwWindow),
      m_queueFamilyIndices(createInfo.queueFamilyIndices) {
  int width, height;
  glfwGetFramebufferSize(m_glfwWindow, &width, &height);

  UpdateInfo(width, height);
  CreateSwapChain();
}

void
VulkanSwapChain::UpdateInfo(uint32_t width, uint32_t height) {
  m_capabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
  auto surfaceFormats = m_physicalDevice.getSurfaceFormatsKHR(m_surface);
  m_surfaceFormat = surfaceFormats[0];
  auto formatResultIter =
      std::find_if(surfaceFormats.cbegin(), surfaceFormats.cend(), [](const auto& surfaceFormat) {
        return surfaceFormat.format == vk::Format::eR8G8B8A8Srgb ||
               surfaceFormat.format == vk::Format::eB8G8R8Srgb;
      });
  if (formatResultIter != surfaceFormats.cend()) {
    m_surfaceFormat = *formatResultIter;
  }

  if (m_capabilities.currentExtent.width == 0xffffffff) {
    const auto& minImageExtent = m_capabilities.minImageExtent;
    const auto& maxImageExtent = m_capabilities.maxImageExtent;
    width = std::clamp<uint32_t>(width, minImageExtent.width, maxImageExtent.width);
    height = std::clamp<uint32_t>(height, minImageExtent.height, maxImageExtent.height);
  } else {
    width = m_capabilities.currentExtent.width;
    height = m_capabilities.currentExtent.height;
  }
  m_width = width;
  m_height = height;

  m_imageCount = 2;

  auto presentModes = m_physicalDevice.getSurfacePresentModesKHR(m_surface);
  m_presentMode = vk::PresentModeKHR::eFifo;
  if (std::find(presentModes.cbegin(), presentModes.cend(), vk::PresentModeKHR::eMailbox) !=
      presentModes.cend()) {
    m_presentMode = vk::PresentModeKHR::eMailbox;
  }
}

VulkanSwapChain::~VulkanSwapChain() {
  m_device.waitIdle();
  for (int i = 0; i < m_imageViews.size(); i++) {
    m_device.destroyImageView(m_imageViews[i]);
  }
  m_device.destroySwapchainKHR(m_swapChain);
}

void
VulkanSwapChain::Update(uint32_t width, uint32_t height) {
  m_device.waitIdle();

  for (int i = 0; i < m_imageViews.size(); i++) {
    m_device.destroyImageView(m_imageViews[i]);
  }
  m_device.destroySwapchainKHR(m_swapChain);

  UpdateInfo(width, height);
  CreateSwapChain();
}

void
VulkanSwapChain::CreateSwapChain() {
  vk::SwapchainCreateInfoKHR swapChainCreateInfo;
  swapChainCreateInfo.setImageColorSpace(m_surfaceFormat.colorSpace);
  swapChainCreateInfo.setImageFormat(m_surfaceFormat.format);
  swapChainCreateInfo.setImageExtent(vk::Extent2D(m_width, m_height));
  swapChainCreateInfo.setMinImageCount(m_imageCount);
  swapChainCreateInfo.setPresentMode(m_presentMode);
  swapChainCreateInfo.setPreTransform(m_capabilities.currentTransform);
  swapChainCreateInfo.setClipped(true);
  swapChainCreateInfo.setSurface(m_surface);
  swapChainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
  swapChainCreateInfo.setImageArrayLayers(1);
  swapChainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

  DLOG_ASSERT(!m_queueFamilyIndices.empty());
  if (m_queueFamilyIndices.size() == 1) {
    swapChainCreateInfo.setQueueFamilyIndices(m_queueFamilyIndices);
    swapChainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
  } else if (m_queueFamilyIndices.size() > 1) {
    swapChainCreateInfo.setQueueFamilyIndices(m_queueFamilyIndices);
    swapChainCreateInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
  }
  m_swapChain = m_device.createSwapchainKHR(swapChainCreateInfo);

  // create image and image view
  m_images = m_device.getSwapchainImagesKHR(m_swapChain);
  m_imageViews.clear();
  for (int i = 0; i < m_images.size(); i++) {
    vk::ImageViewCreateInfo imageViewCreateInfo;
    imageViewCreateInfo.setImage(m_images[i]);
    imageViewCreateInfo.setFormat(m_surfaceFormat.format);
    imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);

    vk::ComponentMapping componentMapping;
    componentMapping.setR(vk::ComponentSwizzle::eR);
    componentMapping.setG(vk::ComponentSwizzle::eG);
    componentMapping.setB(vk::ComponentSwizzle::eB);
    componentMapping.setA(vk::ComponentSwizzle::eA);
    imageViewCreateInfo.setComponents(componentMapping);

    vk::ImageSubresourceRange range;
    range.setAspectMask(vk::ImageAspectFlagBits::eColor);
    range.setBaseArrayLayer(0);
    range.setLayerCount(1);
    range.setBaseMipLevel(0);
    range.setLevelCount(1);
    imageViewCreateInfo.setSubresourceRange(range);

    m_imageViews.push_back(m_device.createImageView(imageViewCreateInfo));
  }
}

}  // namespace Marbas
