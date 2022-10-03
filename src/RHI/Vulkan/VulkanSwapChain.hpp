#pragma once

#include <vulkan/vulkan.hpp>

#include "Common/Common.hpp"
#include "GLFW/glfw3.h"
#include "RHI/Interface/SwapChain.hpp"

namespace Marbas {

struct VulkanSwapChainCreateInfo {
  vk::PhysicalDevice physicalDevice;
  vk::Device device;
  vk::SurfaceKHR surface;
  Vector<uint32_t> queueFamilyIndices;
  GLFWwindow* glfwWindow;
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

  void
  Present() override {}

  void
  Update(uint32_t width, uint32_t height);

 public:
  vk::SurfaceKHR
  GetSurface() const {
    return m_surface;
  }

  vk::SurfaceFormatKHR
  GetSurfaceFormat() const {
    return m_surfaceFormat;
  }

  size_t
  GetImageCount() const {
    return m_images.size();
  }

  vk::Image
  GetImage(int index) const {
    return m_images[index];
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
  GLFWwindow* m_glfwWindow;
  vk::SwapchainKHR m_swapChain;
  vk::SurfaceKHR m_surface;
  vk::Device m_device;
  vk::PhysicalDevice m_physicalDevice;
  vk::SurfaceFormatKHR m_surfaceFormat;
  vk::SurfaceCapabilitiesKHR m_capabilities;
  vk::PresentModeKHR m_presentMode;
  uint32_t m_imageCount;
  Vector<uint32_t> m_queueFamilyIndices;

  Vector<vk::Image> m_images;
  Vector<vk::ImageView> m_imageViews;
};

}  // namespace Marbas
