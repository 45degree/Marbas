#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "RHI/Interface/ImguiInterface.hpp"
#include "RHI/Vulkan/VulkanSwapChain.hpp"

namespace Marbas {

struct VulkanImgui_Impl;

struct VulkanImguiCreateInfo {
  GLFWwindow* glfwWindow = nullptr;
  vk::Instance instance;
  vk::PhysicalDevice physicalDevice;
  uint32_t graphicsQueueFamilyIndex = 0;
  vk::Queue graphicsQueue;
  vk::Device device;
  std::shared_ptr<VulkanSwapChain> swapChain;
};

class VulkanImgui final : public ImguiInterface {
 public:
  explicit VulkanImgui(const VulkanImguiCreateInfo& createInfo);
  virtual ~VulkanImgui() = default;

 public:
  void
  CreateImguiContext() override;

  void
  ClearUp() override;

  void
  SetUpImguiBackend(GLFWwindow* windows) override;

  void
  NewFrame() override;

  void
  RenderData(uint32_t width, uint32_t height) override;

  void
  Resize() override;

 protected:
  void
  Init();

  void
  SetUpVulkanWindowData(bool clearEnable = true);

  void
  CreateWindowCommandBuffer();

  void
  FrameRender();

  void
  FramePresent();

 private:
  GLFWwindow* m_glfwWindow = nullptr;
  const vk::Instance m_instance;
  const vk::PhysicalDevice m_physicalDevice;
  const uint32_t m_graphicsQueueFamilyIndex;
  const vk::Queue m_graphicsQueue;

  vk::Device m_device;
  std::shared_ptr<VulkanSwapChain> m_swapChain;
  // Vector<vk::Framebuffer> m_frameBuffers;
};

}  // namespace Marbas
