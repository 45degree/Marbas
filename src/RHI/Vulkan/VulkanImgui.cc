#include "RHI/Vulkan/VulkanImgui.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <vulkan/vulkan.hpp>

#include "RHI/Interface/ImguiInterface.hpp"
#include "glog/logging.h"

namespace Marbas {

static vk::DescriptorPool descriptorPool;
static ImGui_ImplVulkanH_Window windowData;
static VkAllocationCallbacks* allocator = nullptr;
static bool g_SwapChainRebuild = false;

static vk::RenderPass
CreateImguiRenderPass(const vk::Device& device, const vk::SurfaceFormatKHR& surfaceFormat,
                      bool clearEnable) {
  vk::AttachmentDescription attachment;
  attachment.format = surfaceFormat.format;
  attachment.samples = vk::SampleCountFlagBits::e1;
  attachment.loadOp = clearEnable ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eDontCare;
  attachment.storeOp = vk::AttachmentStoreOp::eStore;
  attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
  attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
  attachment.initialLayout = vk::ImageLayout::eUndefined;
  attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

  vk::AttachmentReference color_attachment;
  color_attachment.attachment = 0;
  color_attachment.layout = vk::ImageLayout::eColorAttachmentOptimal;

  vk::SubpassDescription subpass;
  subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment;
  vk::SubpassDependency dependency;
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
  dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
  dependency.srcAccessMask = vk::AccessFlagBits::eNone;
  dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

  vk::RenderPassCreateInfo info;
  info.attachmentCount = 1;
  info.pAttachments = &attachment;
  info.subpassCount = 1;
  info.pSubpasses = &subpass;
  info.dependencyCount = 1;
  info.pDependencies = &dependency;

  return device.createRenderPass(info);
}

static void
ImGuiDestroyFrame(VkDevice device, ImGui_ImplVulkanH_Frame* fd,
                  const VkAllocationCallbacks* allocator) {
  vkDestroyFence(device, fd->Fence, allocator);
  vkFreeCommandBuffers(device, fd->CommandPool, 1, &fd->CommandBuffer);
  vkDestroyCommandPool(device, fd->CommandPool, allocator);
  fd->Fence = VK_NULL_HANDLE;
  fd->CommandBuffer = VK_NULL_HANDLE;
  fd->CommandPool = VK_NULL_HANDLE;

  // vkDestroyImageView(device, fd->BackbufferView, allocator);
  vkDestroyFramebuffer(device, fd->Framebuffer, allocator);
}

static void
ImGuiDestroyFrameSemaphores(VkDevice device, ImGui_ImplVulkanH_FrameSemaphores* fsd,
                            const VkAllocationCallbacks* allocator) {
  vkDestroySemaphore(device, fsd->ImageAcquiredSemaphore, allocator);
  vkDestroySemaphore(device, fsd->RenderCompleteSemaphore, allocator);
  fsd->ImageAcquiredSemaphore = fsd->RenderCompleteSemaphore = VK_NULL_HANDLE;
}

void
VulkanImgui::FrameRender() {
  VkResult err;
  ImDrawData* draw_data = ImGui::GetDrawData();

  VkSemaphore image_acquired_semaphore =
      windowData.FrameSemaphores[windowData.SemaphoreIndex].ImageAcquiredSemaphore;
  VkSemaphore render_complete_semaphore =
      windowData.FrameSemaphores[windowData.SemaphoreIndex].RenderCompleteSemaphore;
  err = vkAcquireNextImageKHR(m_device, windowData.Swapchain, UINT64_MAX, image_acquired_semaphore,
                              VK_NULL_HANDLE, &windowData.FrameIndex);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
    g_SwapChainRebuild = true;
    LOG(INFO) << "need rebuild swap chain";
    return;
  }
  DLOG_ASSERT(err == VK_SUCCESS);

  ImGui_ImplVulkanH_Frame* fd = &windowData.Frames[windowData.FrameIndex];
  err = vkWaitForFences(m_device, 1, &fd->Fence, VK_TRUE,
                        UINT64_MAX);  // wait indefinitely instead of periodically checking
  DLOG_ASSERT(err == VK_SUCCESS);

  err = vkResetFences(m_device, 1, &fd->Fence);
  DLOG_ASSERT(err == VK_SUCCESS);

  err = vkResetCommandPool(m_device, fd->CommandPool, 0);
  DLOG_ASSERT(err == VK_SUCCESS);
  {
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
    DLOG_ASSERT(err == VK_SUCCESS);
  }

  {
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = windowData.RenderPass;
    info.framebuffer = fd->Framebuffer;
    info.renderArea.extent.width = windowData.Width;
    info.renderArea.extent.height = windowData.Height;
    info.clearValueCount = 1;
    info.pClearValues = &windowData.ClearValue;
    vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
  }

  // Record dear imgui primitives into command buffer
  ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

  // Submit command buffer
  vkCmdEndRenderPass(fd->CommandBuffer);

  {
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &image_acquired_semaphore;
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &fd->CommandBuffer;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &render_complete_semaphore;

    err = vkEndCommandBuffer(fd->CommandBuffer);
    DLOG_ASSERT(err == VK_SUCCESS);
    err = vkQueueSubmit(m_graphicsQueue, 1, &info, fd->Fence);
    DLOG_ASSERT(err == VK_SUCCESS);
  }
}

void
VulkanImgui::FramePresent() {
  if (g_SwapChainRebuild) return;
  VkSemaphore render_complete_semaphore =
      windowData.FrameSemaphores[windowData.SemaphoreIndex].RenderCompleteSemaphore;
  VkPresentInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &render_complete_semaphore;
  info.swapchainCount = 1;
  info.pSwapchains = &windowData.Swapchain;
  info.pImageIndices = &windowData.FrameIndex;
  VkResult err = vkQueuePresentKHR(m_graphicsQueue, &info);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
    g_SwapChainRebuild = true;
    return;
  }
  DLOG_ASSERT(err == VK_SUCCESS);
  windowData.SemaphoreIndex = (windowData.SemaphoreIndex + 1) %
                              windowData.ImageCount;  // Now we can use the next set of semaphores
}

void
VulkanImgui::SetUpVulkanWindowData(bool clearEnable) {
  if (static_cast<VkDescriptorPool>(descriptorPool) != nullptr) {
    m_device.destroyDescriptorPool(descriptorPool);
  }
  // some uniforms in vulkan shader.
  const uint32_t maxSize = 1000;
  std::array<vk::DescriptorPoolSize, 11> poolSizes = {
      vk::DescriptorPoolSize{vk::DescriptorType::eSampler, maxSize},
      vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, maxSize},
      vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage, maxSize},
      vk::DescriptorPoolSize{vk::DescriptorType::eStorageImage, maxSize},
      vk::DescriptorPoolSize{vk::DescriptorType::eUniformTexelBuffer, maxSize},
      vk::DescriptorPoolSize{vk::DescriptorType::eStorageTexelBuffer, maxSize},
      vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, maxSize},
      vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer, maxSize},
      vk::DescriptorPoolSize{vk::DescriptorType::eUniformBufferDynamic, maxSize},
      vk::DescriptorPoolSize{vk::DescriptorType::eStorageBufferDynamic, maxSize},
      vk::DescriptorPoolSize{vk::DescriptorType::eInputAttachment, maxSize},
  };
  vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
  descriptorPoolCreateInfo.setPoolSizes(poolSizes);
  descriptorPoolCreateInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
  descriptorPoolCreateInfo.setMaxSets(maxSize * poolSizes.size());
  descriptorPool = m_device.createDescriptorPool(descriptorPoolCreateInfo);

  auto height = m_swapChain->GetHeight();
  auto width = m_swapChain->GetWidth();
  auto imageCount = m_swapChain->GetImageCount();
  auto surface = m_swapChain->GetSurface();
  auto surfaceFormat = m_swapChain->GetSurfaceFormat();

  windowData.Surface = surface;
  windowData.Swapchain = m_swapChain->GetOriginalSwapChain();
  windowData.PresentMode = static_cast<VkPresentModeKHR>(m_swapChain->GetPresentMode());
  windowData.ClearEnable = clearEnable;
  windowData.RenderPass = CreateImguiRenderPass(m_device, surfaceFormat, clearEnable);
  windowData.SurfaceFormat = surfaceFormat;
  windowData.ImageCount = imageCount;
  windowData.Frames = new ImGui_ImplVulkanH_Frame[imageCount];
  windowData.Height = height;
  windowData.Width = width;
  windowData.FrameSemaphores = new ImGui_ImplVulkanH_FrameSemaphores[imageCount];

  for (int i = 0; i < imageCount; i++) {
    auto image = m_swapChain->GetImage(i);
    auto imageView = m_swapChain->GetImageView(i);
    windowData.Frames[i].Backbuffer = image;
    windowData.Frames[i].BackbufferView = imageView;

    // create frame buffer
    vk::FramebufferCreateInfo frameCreateInfo;
    frameCreateInfo.setAttachments(imageView);
    frameCreateInfo.setWidth(width);
    frameCreateInfo.setHeight(height);
    frameCreateInfo.setLayers(1);
    frameCreateInfo.setRenderPass(windowData.RenderPass);
    // m_frameBuffers.push_back(m_device.createFramebuffer(frameCreateInfo));

    windowData.Frames[i].Framebuffer = m_device.createFramebuffer(frameCreateInfo);
  }
}

VulkanImgui::VulkanImgui(const VulkanImguiCreateInfo& createInfo)
    : ImguiInterface(),
      m_glfwWindow(createInfo.glfwWindow),
      m_instance(createInfo.instance),
      m_physicalDevice(createInfo.physicalDevice),
      m_graphicsQueueFamilyIndex(createInfo.graphicsQueueFamilyIndex),
      m_graphicsQueue(createInfo.graphicsQueue),
      m_device(createInfo.device),
      m_swapChain(createInfo.swapChain) {
  SetUpVulkanWindowData(true);
  CreateWindowCommandBuffer();
}

void
VulkanImgui::Init() {}

void
VulkanImgui::NewFrame() {
  // Start the Dear ImGui frame
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void
VulkanImgui::ClearUp() {
  auto err = vkDeviceWaitIdle(m_device);
  DLOG_ASSERT(err == VK_SUCCESS);
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  for (uint32_t i = 0; i < windowData.ImageCount; i++) {
    ImGuiDestroyFrame(m_device, &windowData.Frames[i], allocator);
    ImGuiDestroyFrameSemaphores(m_device, &windowData.FrameSemaphores[i], allocator);
  }
  IM_FREE(windowData.Frames);
  IM_FREE(windowData.FrameSemaphores);
  windowData.Frames = NULL;
  windowData.FrameSemaphores = NULL;
  vkDestroyPipeline(m_device, windowData.Pipeline, allocator);
  vkDestroyRenderPass(m_device, windowData.RenderPass, allocator);
  vkDestroyDescriptorPool(m_device, descriptorPool, allocator);
}

void
VulkanImgui::CreateImguiContext() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
}

void
VulkanImgui::SetUpImguiBackend(GLFWwindow* windows) {
  ImGui_ImplGlfw_InitForVulkan(windows, true);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = m_instance;
  init_info.PhysicalDevice = m_physicalDevice;
  init_info.Device = m_device;
  init_info.QueueFamily = m_graphicsQueueFamilyIndex;
  init_info.Queue = m_graphicsQueue;
  init_info.PipelineCache = nullptr;
  init_info.DescriptorPool = descriptorPool;
  init_info.Subpass = 0;
  init_info.MinImageCount = 2;
  init_info.ImageCount = windowData.ImageCount;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.Allocator = nullptr;
  init_info.CheckVkResultFn = nullptr;
  ImGui_ImplVulkan_Init(&init_info, windowData.RenderPass);

  // upload font

  // Use any command queue
  VkCommandPool command_pool = windowData.Frames[windowData.FrameIndex].CommandPool;
  VkCommandBuffer command_buffer = windowData.Frames[windowData.FrameIndex].CommandBuffer;

  auto err = vkResetCommandPool(m_device, command_pool, 0);
  DLOG_ASSERT(err == VK_SUCCESS);

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  err = vkBeginCommandBuffer(command_buffer, &begin_info);
  DLOG_ASSERT(err == VK_SUCCESS);

  ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

  VkSubmitInfo end_info = {};
  end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  end_info.commandBufferCount = 1;
  end_info.pCommandBuffers = &command_buffer;
  err = vkEndCommandBuffer(command_buffer);
  DLOG_ASSERT(err == VK_SUCCESS);
  err = vkQueueSubmit(m_graphicsQueue, 1, &end_info, VK_NULL_HANDLE);
  DLOG_ASSERT(err == VK_SUCCESS);

  err = vkDeviceWaitIdle(m_device);
  DLOG_ASSERT(err == VK_SUCCESS);

  ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void
VulkanImgui::RenderData(uint32_t, uint32_t) {
  ImGui::Render();
  ImDrawData* draw_data = ImGui::GetDrawData();
  const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
  if (!is_minimized) {
    windowData.ClearValue.color.float32[0] = 0;
    windowData.ClearValue.color.float32[1] = 0;
    windowData.ClearValue.color.float32[2] = 0;
    windowData.ClearValue.color.float32[3] = 0;
    FrameRender();
    FramePresent();
  }
}

void
VulkanImgui::CreateWindowCommandBuffer() {
  // Create Command Buffers
  VkResult err;
  for (uint32_t i = 0; i < windowData.ImageCount; i++) {
    ImGui_ImplVulkanH_Frame* fd = &windowData.Frames[i];
    ImGui_ImplVulkanH_FrameSemaphores* fsd = &windowData.FrameSemaphores[i];
    {
      VkCommandPoolCreateInfo info = {};
      info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
      info.queueFamilyIndex = m_graphicsQueueFamilyIndex;
      err = vkCreateCommandPool(m_device, &info, allocator, &fd->CommandPool);
      DLOG_ASSERT(err == VK_SUCCESS);
    }
    {
      VkCommandBufferAllocateInfo info = {};
      info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      info.commandPool = fd->CommandPool;
      info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      info.commandBufferCount = 1;
      err = vkAllocateCommandBuffers(m_device, &info, &fd->CommandBuffer);
      DLOG_ASSERT(err == VK_SUCCESS);
    }
    {
      VkFenceCreateInfo info = {};
      info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
      err = vkCreateFence(m_device, &info, allocator, &fd->Fence);
      DLOG_ASSERT(err == VK_SUCCESS);
    }
    {
      VkSemaphoreCreateInfo info = {};
      info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
      err = vkCreateSemaphore(m_device, &info, allocator, &fsd->ImageAcquiredSemaphore);
      DLOG_ASSERT(err == VK_SUCCESS);
      err = vkCreateSemaphore(m_device, &info, allocator, &fsd->RenderCompleteSemaphore);
      DLOG_ASSERT(err == VK_SUCCESS);
    }
  }
}

void
VulkanImgui::Resize() {
  if (g_SwapChainRebuild) {
    int width, height;
    glfwGetFramebufferSize(m_glfwWindow, &width, &height);
    if (width > 0 && height > 0) {
      ImGui_ImplVulkan_SetMinImageCount(2);
      // ImGui_ImplVulkanH_CreateOrResizeWindow(m_instance, m_physicalDevice, m_device, &windowData,
      //                                        m_graphicsQueueFamilyIndex, nullptr, width, height,
      //                                        2);

      m_device.waitIdle();
      for (int i = 0; i < windowData.ImageCount; i++) {
        ImGuiDestroyFrame(m_device, &windowData.Frames[i], nullptr);
        ImGuiDestroyFrameSemaphores(m_device, &windowData.FrameSemaphores[i], nullptr);
      }

      // SetUpVulkanWindowData(true);
      // CreateWindowCommandBuffer();

      m_swapChain->Update(width, height);
      windowData.Swapchain = m_swapChain->GetOriginalSwapChain();
      windowData.Width = width;
      windowData.Height = height;

      VkImageView attachment[1];
      VkFramebufferCreateInfo info = {};
      info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      info.renderPass = windowData.RenderPass;
      info.attachmentCount = 1;
      info.pAttachments = attachment;
      info.width = width;
      info.height = height;
      info.layers = 1;
      // ImGui_ImplVulkan_SetMinImageCount(m_swapChain->GetImageCount());

      // m_device.waitIdle();

      for (uint32_t i = 0; i < windowData.ImageCount; i++) {
        windowData.Frames[i].Backbuffer = m_swapChain->GetImage(i);
        windowData.Frames[i].BackbufferView = m_swapChain->GetImageView(i);

        ImGui_ImplVulkanH_Frame* fd = &windowData.Frames[i];
        attachment[0] = m_swapChain->GetImageView(i);
        auto err = vkCreateFramebuffer(m_device, &info, nullptr, &fd->Framebuffer);
        DLOG_ASSERT(err == VK_SUCCESS);
      }
      CreateWindowCommandBuffer();
      windowData.FrameIndex = 0;
      g_SwapChainRebuild = false;
    }
  }
}

}  // namespace Marbas
