#include "RHI/Vulkan/VulkanRHIFactory.hpp"

#include <glog/logging.h>

#include "vulkan/vulkan_structs.hpp"

namespace Marbas {

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
    if (m_physicalDevice.getSurfaceSupportKHR(index, m_surface)) {
      m_presentQueueFamilyIndex = index;
    }
    if (m_graphicsQueueFamilyIndex && m_presentQueueFamilyIndex) break;
    index++;
  }

  // create logical device
  vk::DeviceCreateInfo deviceCreateInfo;
  std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  deviceCreateInfo.setPEnabledExtensionNames(deviceExtensions);
  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
  std::vector<float> priorities = {1.0};
  if (*m_graphicsQueueFamilyIndex == *m_presentQueueFamilyIndex) {
    vk::DeviceQueueCreateInfo createInfo;
    createInfo.setQueueFamilyIndex(*m_graphicsQueueFamilyIndex);

    createInfo.setQueuePriorities(priorities);
    createInfo.setQueueCount(1);
    queueCreateInfos.push_back(createInfo);
  } else {
    vk::DeviceQueueCreateInfo createInfo;

    createInfo.setQueueCount(1);
    createInfo.setQueuePriorities(priorities);
    createInfo.setQueueFamilyIndex(*m_graphicsQueueFamilyIndex);
    queueCreateInfos.push_back(createInfo);

    createInfo.setQueueFamilyIndex(*m_presentQueueFamilyIndex);
    queueCreateInfos.push_back(createInfo);
  }

  deviceCreateInfo.setQueueCreateInfos(queueCreateInfos);
  m_device = m_physicalDevice.createDevice(deviceCreateInfo);

  m_graphicsQueue = m_device.getQueue(*m_graphicsQueueFamilyIndex, 0);
  m_presentQueue = m_device.getQueue(*m_presentQueueFamilyIndex, 0);

  // create swapChain
  VulkanSwapChainCreateInfo createInfo;
  createInfo.device = m_device;
  createInfo.surface = m_surface;
  createInfo.glfwWindow = m_glfwWindow;
  createInfo.physicalDevice = m_physicalDevice;
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
