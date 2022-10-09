#pragma once

#include <span>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

#include "Common/Common.hpp"
#include "GLFW/glfw3.h"
#include "RHI/Interface/Semaphore.hpp"
#include "RHI/Interface/SwapChain.hpp"
#include "RHI/Vulkan/VulkanTexture.hpp"

namespace Marbas {

struct VulkanSwapChainCreateInfo {
  vk::PhysicalDevice physicalDevice;
  vk::Device device;
  vk::SurfaceKHR surface;
  Vector<uint32_t> queueFamilyIndices;
  uint32_t transferQueueFamilyIndex = 0;
  vk::Queue transferQueue;
  vk::Queue presentQueue;
};

class VulkanSwapChain final : public SwapChain {
 public:
  explicit VulkanSwapChain(const VulkanSwapChainCreateInfo& createInfo);
  ~VulkanSwapChain();

 public:
  std::shared_ptr<FrameBuffer>
  GetDefaultFrameBuffer() override {
    return nullptr;
  }

  /**
   * @brief present the image to the surface
   *
   * @param waitSemaphores indicate the render has finshed
   * @return
   */
  int
  Present(const Vector<Semaphore>& waitSemaphores, uint32_t imageIndex) override;

  /**
   * @brief
   *
   * @param semaphore
   * @return
   */
  int
  AcquireNextImage(const Semaphore& semaphore) override;

  size_t
  GetImageCount() const override {
    return m_images.size();
  }

  void
  Resize(uint32_t width, uint32_t height) override;

 public:
  vk::SurfaceKHR
  GetSurface() const {
    return m_surface;
  }

  vk::SurfaceFormatKHR
  GetSurfaceFormat() const {
    return m_surfaceFormat;
  }

  vk::Image
  GetImage(int index) const {
    return m_images[index].GetImageHandle<vk::Image>();
  }

  vk::ImageView
  GetImageView(int index) const {
    return m_imageViews[index];
  }

  vk::PresentModeKHR
  GetPresentMode() const {
    return m_presentMode;
  }

  vk::SwapchainKHR
  GetOriginalSwapChain() const {
    return m_swapChain;
  }

 private:
  void
  CreateSwapChain();

  void
  UpdateInfo(uint32_t width, uint32_t height);

 private:
  vk::SwapchainKHR m_swapChain;
  vk::SurfaceKHR m_surface;
  vk::Device m_device;
  vk::PhysicalDevice m_physicalDevice;
  vk::SurfaceFormatKHR m_surfaceFormat;
  vk::SurfaceCapabilitiesKHR m_capabilities;
  vk::PresentModeKHR m_presentMode;
  vk::Queue m_presentQueue;
  uint32_t m_imageCount;
  Vector<uint32_t> m_queueFamilyIndices;

  // swapChain image
  Vector<VulkanTexture> m_images;
  Vector<vk::ImageView> m_imageViews;
};

}  // namespace Marbas
