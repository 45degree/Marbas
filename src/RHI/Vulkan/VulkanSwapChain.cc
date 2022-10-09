#include "RHI/Vulkan/VulkanSwapChain.hpp"

#include "RHI/Interface/Semaphore.hpp"
#include "glog/logging.h"

namespace Marbas {
static void
insertImageMemoryBarrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask,
                         VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout,
                         VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask,
                         VkPipelineStageFlags dstStageMask,
                         VkImageSubresourceRange subresourceRange) {
  VkImageMemoryBarrier imageMemoryBarrier;
  imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  imageMemoryBarrier.pNext = NULL;
  imageMemoryBarrier.srcAccessMask = srcAccessMask;
  imageMemoryBarrier.dstAccessMask = dstAccessMask;
  imageMemoryBarrier.oldLayout = oldImageLayout;
  imageMemoryBarrier.newLayout = newImageLayout;
  imageMemoryBarrier.image = image;
  imageMemoryBarrier.subresourceRange = subresourceRange;
  imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

  vkCmdPipelineBarrier(cmdbuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1,
                       &imageMemoryBarrier);
}

VulkanSwapChain::VulkanSwapChain(const VulkanSwapChainCreateInfo& createInfo)
    : SwapChain(800, 600),
      m_device(createInfo.device),
      m_physicalDevice(createInfo.physicalDevice),
      m_surface(createInfo.surface),
      m_queueFamilyIndices(createInfo.queueFamilyIndices),
      m_presentQueue(createInfo.presentQueue) {
  UpdateInfo(m_width, m_height);
  CreateSwapChain();
}

int
VulkanSwapChain::AcquireNextImage(const Semaphore& semaphore) {
  auto currentSemaphore = semaphore.GetHandler<vk::Semaphore>();
  auto nextImageResult = m_device.acquireNextImageKHR(m_swapChain, UINT64_MAX, currentSemaphore);
  if (nextImageResult.result == vk::Result::eErrorOutOfDateKHR ||
      nextImageResult.result == vk::Result::eSuboptimalKHR) {
    // TODO: need add a handler to notify other should change the size
    return -1;
  }
  DLOG_ASSERT(nextImageResult.result == vk::Result::eSuccess);
  return nextImageResult.value;
}

int
VulkanSwapChain::Present(const Vector<Semaphore>& waitSemaphores, uint32_t imageIndex) {
  std::vector<vk::Semaphore> semaphores;
  for (const auto& waitSemaphore : waitSemaphores) {
    semaphores.push_back(waitSemaphore.GetHandler<vk::Semaphore>());
  }

  vk::PresentInfoKHR presentInfo;
  presentInfo.setWaitSemaphores(semaphores);
  presentInfo.setSwapchains(m_swapChain);
  presentInfo.setImageIndices(imageIndex);
  auto err2 = m_presentQueue.presentKHR(&presentInfo);

  if (err2 == vk::Result::eErrorOutOfDateKHR || err2 == vk::Result::eSuboptimalKHR) {
    // TODO: need add a handler to notify other should change the size
    return -1;
  }
  DLOG_ASSERT(err2 == vk::Result::eSuccess);
  return 1;
}

void
VulkanSwapChain::UpdateInfo(uint32_t width, uint32_t height) {
  m_capabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
  auto surfaceFormats = m_physicalDevice.getSurfaceFormatsKHR(m_surface);
  m_surfaceFormat = surfaceFormats[0];
  auto formatResultIter =
      std::find_if(surfaceFormats.cbegin(), surfaceFormats.cend(), [](const auto& surfaceFormat) {
        return surfaceFormat.format == vk::Format::eR8G8B8A8Unorm ||
               surfaceFormat.format == vk::Format::eB8G8R8A8Unorm ||
               surfaceFormat.format == vk::Format::eR8G8B8Unorm ||
               surfaceFormat.format == vk::Format::eB8G8R8Unorm;
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
VulkanSwapChain::Resize(uint32_t width, uint32_t height) {
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
  auto images = m_device.getSwapchainImagesKHR(m_swapChain);
  m_images.clear();
  for (const auto& image : images) {
    auto handler = reinterpret_cast<uintptr_t>(static_cast<VkImage>(image));
    m_images.emplace_back(handler);
  }
  m_imageViews.clear();
  for (int i = 0; i < m_images.size(); i++) {
    vk::ImageViewCreateInfo imageViewCreateInfo;
    imageViewCreateInfo.setImage(m_images[i].GetImageHandle<vk::Image>());
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
