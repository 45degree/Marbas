#pragma once

#include <vulkan/vulkan.hpp>

#include "RHI/Interface/RHIFactory.hpp"
#include "RHI/Vulkan/VulkanImgui.hpp"
#include "RHI/Vulkan/VulkanSwapChain.hpp"

namespace Marbas {

class VulkanRHIFactory final : public RHIFactory {
 public:
  explicit VulkanRHIFactory() = default;
  virtual ~VulkanRHIFactory();

  VulkanRHIFactory(const VulkanRHIFactory&) = delete;
  VulkanRHIFactory(VulkanRHIFactory&&) = delete;
  VulkanRHIFactory&
  operator=(const VulkanRHIFactory&) = delete;
  VulkanRHIFactory&
  operator=(VulkanRHIFactory&&) = delete;

 public:
  void
  Init(const RHICreateInfo& extraInfo) override;

  std::unique_ptr<ImguiInterface>
  CreateImguiInterface() const override {
    VulkanImguiCreateInfo imguiCreateInfo;
    imguiCreateInfo.glfwWindow = m_glfwWindow;
    imguiCreateInfo.physicalDevice = m_physicalDevice;
    imguiCreateInfo.instance = m_instance;
    imguiCreateInfo.swapChain = m_swapchain;
    imguiCreateInfo.device = m_device;
    imguiCreateInfo.graphicsQueue = m_graphicsQueue;
    imguiCreateInfo.graphicsQueueFamilyIndex = *m_graphicsQueueFamilyIndex;

    return std::make_unique<VulkanImgui>(imguiCreateInfo);
  }

  std::unique_ptr<VertexBuffer>
  CreateVertexBuffer(const void* data, size_t size) const override {
    return nullptr;
  }

  std::unique_ptr<VertexBuffer>
  CreateVertexBuffer(size_t size) const override {
    return nullptr;
  }

  std::unique_ptr<IndexBuffer>
  CreateIndexBuffer(const Vector<uint32_t>& indices) const override {
    return nullptr;
  }

  std::unique_ptr<ShaderStage>
  CreateShaderStage(const ShaderType shaderType) const override {
    return nullptr;
  }

  std::unique_ptr<Shader>
  CreateShader() const override {
    return nullptr;
  }

  std::shared_ptr<SwapChain>
  GetSwapChain() override {
    return m_swapchain;
  }

  std::unique_ptr<RenderPass>
  CreateRenderPass(const RenderPassCreateInfo& createInfo) override {
    return nullptr;
  }

  std::unique_ptr<GraphicsPipeLine>
  CreateGraphicsPipeLine() override {
    return nullptr;
  }

  std::unique_ptr<Texture>
  CreateTexture(const ImageDesc& imageDesc) const override {
    return nullptr;
  }

  std::unique_ptr<Texture>
  CreateTexture2D(const Path& imagePath, uint32_t levels, bool isHDR) const override {
    return nullptr;
  }

  std::unique_ptr<Texture>
  CreateTextureCubeMap(const CubeMapCreateInfo& createInfo, uint32_t levels) const override {
    return nullptr;
  }

  std::unique_ptr<ImageView>
  CreateImageView(const ImageViewDesc& imageViewDesc) const override {
    return nullptr;
  }

  std::unique_ptr<CommandBuffer>
  CreateCommandBuffer() const override {
    return nullptr;
  }

  std::unique_ptr<UniformBuffer>
  CreateUniformBuffer(uint32_t size) const override {
    return nullptr;
  }

  std::unique_ptr<DynamicUniformBuffer>
  CreateDynamicUniforBuffer(uint32_t size) const override {
    return nullptr;
  }

  std::unique_ptr<FrameBuffer>
  CreateFrameBuffer(const FrameBufferInfo& info) const override {
    return nullptr;
  }

  std::unique_ptr<DescriptorSet>
  CreateDescriptorSet(const DescriptorSetLayout& createInfo) const override {
    return nullptr;
  }

 private:
  vk::Device m_device;
  vk::PhysicalDevice m_physicalDevice;

  std::optional<uint32_t> m_graphicsQueueFamilyIndex = std::nullopt;
  std::optional<uint32_t> m_presentQueueFamilyIndex = std::nullopt;

  vk::Queue m_graphicsQueue;
  vk::Queue m_presentQueue;

  vk::Instance m_instance;
  vk::SurfaceKHR m_surface;

  std::shared_ptr<VulkanSwapChain> m_swapchain;
};

}  // namespace Marbas
