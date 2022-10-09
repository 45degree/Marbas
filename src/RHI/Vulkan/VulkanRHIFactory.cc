#include "RHI/Vulkan/VulkanRHIFactory.hpp"

#include <glog/logging.h>

#include "vulkan/vulkan_structs.hpp"

namespace Marbas {

VulkanRHIFactory::VulkanRHIFactory() { glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); }

void
VulkanRHIFactory::Init(const RHICreateInfo& extraInfo) {
  // get glfw extensions
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  // validation layers
  std::array<const char*, 1> layers = {"VK_LAYER_KHRONOS_validation"};

  // create vulkan instance
  vk::InstanceCreateInfo instanceCreateInfo;
  instanceCreateInfo.setEnabledExtensionCount(glfwExtensionCount);
  instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;
  instanceCreateInfo.setEnabledLayerCount(layers.size());
  instanceCreateInfo.setPpEnabledLayerNames(layers.data());

  m_instance = vk::createInstance(instanceCreateInfo);
  VkInstance instance = static_cast<VkInstance>(m_instance);

  // set surface KHR
  VkSurfaceKHR surface;
  if (glfwCreateWindowSurface(instance, m_glfwWindow, nullptr, &surface) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
  m_surface = static_cast<vk::SurfaceKHR>(surface);

  // create physiacal device
  auto physicalDevices = m_instance.enumeratePhysicalDevices();
  if (physicalDevices.size() == 1) {
    m_physicalDevice = physicalDevices[0];
  } else {
    for (auto device : physicalDevices) {
      m_physicalDevice = device;
      auto properties = device.getProperties();
      if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        m_physicalDevice = device;
        break;
      }
    }
  }
  auto properties = m_physicalDevice.getProperties();
  LOG(INFO) << FORMAT("find the physical device: {}, version: {}, api version: {}",
                      properties.deviceName, properties.driverVersion, properties.apiVersion);

  // fina queue family
  auto families = m_physicalDevice.getQueueFamilyProperties();
  uint32_t index = 0;
  for (const auto& family : families) {
    if (family.queueFlags | vk::QueueFlagBits::eGraphics) {
      m_graphicsQueueFamilyIndex = index;
    }
    if (family.queueFlags | vk::QueueFlagBits::eTransfer) {
      m_transferQueueFamilyIndex = index;
    }
    if (m_physicalDevice.getSurfaceSupportKHR(index, m_surface)) {
      m_presentQueueFamilyIndex = index;
    }
    if (m_graphicsQueueFamilyIndex && m_presentQueueFamilyIndex && m_transferQueueFamilyIndex)
      break;
    index++;
  }

  /**
   * create logical device and queue
   */
  vk::DeviceCreateInfo deviceCreateInfo;
  std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  deviceCreateInfo.setPEnabledExtensionNames(deviceExtensions);
  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
  std::array<float, 1> priorities = {1.0};

  // create graphics queue create info
  vk::DeviceQueueCreateInfo queueCreateInfo;
  queueCreateInfo.setQueueFamilyIndex(*m_graphicsQueueFamilyIndex);
  queueCreateInfo.setQueuePriorities(priorities);
  queueCreateInfo.setQueueCount(1);
  queueCreateInfos.push_back(queueCreateInfo);

  if (*m_presentQueueFamilyIndex != *m_graphicsQueueFamilyIndex) {
    queueCreateInfo.setQueueFamilyIndex(*m_presentQueueFamilyIndex);
    queueCreateInfos.push_back(queueCreateInfo);
  }

  if (*m_transferQueueFamilyIndex != *m_graphicsQueueFamilyIndex &&
      *m_transferQueueFamilyIndex != *m_presentQueueFamilyIndex) {
    queueCreateInfo.setQueueFamilyIndex(*m_transferQueueFamilyIndex);
    queueCreateInfos.push_back(queueCreateInfo);
  }

  // create device and queue
  deviceCreateInfo.setQueueCreateInfos(queueCreateInfos);
  m_device = m_physicalDevice.createDevice(deviceCreateInfo);

  m_graphicsQueue = m_device.getQueue(*m_graphicsQueueFamilyIndex, 0);
  m_presentQueue = m_device.getQueue(*m_presentQueueFamilyIndex, 0);
  m_transferQueue = m_device.getQueue(*m_transferQueueFamilyIndex, 0);

  // create swapChain
  VulkanSwapChainCreateInfo createInfo;
  createInfo.device = m_device;
  createInfo.surface = m_surface;
  createInfo.physicalDevice = m_physicalDevice;
  createInfo.presentQueue = m_presentQueue;
  createInfo.transferQueue = m_transferQueue;
  createInfo.transferQueueFamilyIndex = *m_transferQueueFamilyIndex;
  if (*m_graphicsQueueFamilyIndex == *m_presentQueueFamilyIndex) {
    createInfo.queueFamilyIndices = {*m_graphicsQueueFamilyIndex};
  } else {
    createInfo.queueFamilyIndices = {*m_graphicsQueueFamilyIndex, *m_presentQueueFamilyIndex};
  }
  m_swapchain = std::make_shared<VulkanSwapChain>(createInfo);
}

VulkanRHIFactory::~VulkanRHIFactory() {
  // delete swap chain
  m_swapchain = nullptr;

  m_device.destroy();
  m_instance.destroySurfaceKHR(m_surface);
  m_instance.destroy();
}

}  // namespace Marbas
