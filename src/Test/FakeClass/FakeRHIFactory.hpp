#include <gmock/gmock.h>

#include "RHIFactory.hpp"

namespace Marbas {

class MockComputeCommandBuffer final : public ComputeCommandBuffer {
  MOCK_METHOD(void, Begin, ());
  MOCK_METHOD(void, End, ());
  MOCK_METHOD(void, Submit, (std::span<Semaphore*>, std::span<Semaphore*>, Fence*));
  MOCK_METHOD(void, BeginPipeline, (uintptr_t));
  MOCK_METHOD(void, EndPipeline, (uintptr_t));
  MOCK_METHOD(void, BindDescriptorSet, (uintptr_t, const std::vector<uintptr_t>&));
  MOCK_METHOD(void, ClearColorImage, (Image*, const ClearValue&, int, int, int, int));
  MOCK_METHOD(void, Dispatch, (uint32_t, uint32_t, uint32_t));
};

class MockGraphicsCommandBuffer final : public GraphicsCommandBuffer {
  MOCK_METHOD(void, Begin, ());
  MOCK_METHOD(void, End, ());
  MOCK_METHOD(void, Submit, (std::span<Semaphore*>, std::span<Semaphore*>, Fence*));
  MOCK_METHOD(void, BeginPipeline, (uintptr_t, FrameBuffer*, const std::vector<ClearValue>&));
  MOCK_METHOD(void, EndPipeline, (uintptr_t));
  MOCK_METHOD(void, BindDescriptorSet, (uintptr_t, const std::vector<uintptr_t>&));
  MOCK_METHOD(void, PushConstant, (uintptr_t, const void*, uint32_t, uint32_t));

  MOCK_METHOD(void, SetViewports, (std::span<ViewportInfo>));
  MOCK_METHOD(void, SetScissors, (std::span<ScissorInfo>));

  MOCK_METHOD(void, SetCullMode, (CullMode));
  MOCK_METHOD(void, BindVertexBuffer, (Buffer*));
  MOCK_METHOD(void, BindIndexBuffer, (Buffer*));
  MOCK_METHOD(void, Draw, (uint32_t, uint32_t, uint32_t, uint32_t));
  MOCK_METHOD(void, DrawIndexed, (uint32_t, uint32_t, uint32_t, int32_t, uint32_t));
  MOCK_METHOD(void, GenerateMipmap, (Image*, uint32_t));
  MOCK_METHOD(void, ClearColorImage, (Image*, const ClearValue&, int, int, int, int));
};

class MockPipelineContext final : public PipelineContext {
 public:
  MOCK_METHOD(uintptr_t, CreatePipeline, (const GraphicsPipeLineCreateInfo&));
  MOCK_METHOD(uintptr_t, CreatePipeline, (const ComputePipelineCreateInfo&));
  MOCK_METHOD(void, DestroyPipeline, (uintptr_t));
  MOCK_METHOD(uintptr_t, CreateSampler, (const SamplerCreateInfo&));
  MOCK_METHOD(void, DestroySampler, (uintptr_t));
  MOCK_METHOD(uintptr_t, CreateDescriptorSet, (const DescriptorSetArgument&));
  MOCK_METHOD(void, DestroyDescriptorSet, (uintptr_t));
  MOCK_METHOD(void, BindImage, (const BindImageInfo&));
  MOCK_METHOD(void, BindStorageImage, (const BindStorageImageInfo&));
  MOCK_METHOD(void, BindBuffer, (const BindBufferInfo&));
  MOCK_METHOD(FrameBuffer*, CreateFrameBuffer, (const FrameBufferCreateInfo&));
  MOCK_METHOD(void, DestroyFrameBuffer, (FrameBuffer*));
};

class MockBufferContext final : public BufferContext {
 public:
  MOCK_METHOD(Buffer*, CreateBuffer, (BufferType, const void*, uint32_t, bool));
  MOCK_METHOD(void, UpdateBuffer, (Buffer*, const void*, uint32_t, uintptr_t));
  MOCK_METHOD(void, DestroyBuffer, (Buffer*));
  MOCK_METHOD(void, GetBufferData, (Buffer*, void*));
  MOCK_METHOD(TexelBuffer*, CreateTexelBuffer, (Buffer*, ImageFormat));
  MOCK_METHOD(void, DestroyTexelBuffer, (TexelBuffer*));
  MOCK_METHOD(Image*, CreateImage, (const ImageCreateInfo&));
  MOCK_METHOD(void, UpdateImage, (const UpdateImageInfo&));
  MOCK_METHOD(void, GetImageData, (const ImageSubresourceDesc&, void*));
  MOCK_METHOD(uint32_t, GetImageSubresourceSize, (const ImageSubresourceDesc&));
  MOCK_METHOD(void, GenerateMipmap, (Image*, uint32_t));
  MOCK_METHOD(void, DestroyImage, (Image*));
  MOCK_METHOD(ImageView*, CreateImageView, (const ImageViewCreateInfo&));
  MOCK_METHOD(void, DestroyImageView, (ImageView*));
  MOCK_METHOD(GraphicsCommandBuffer*, CreateGraphicsCommandBuffer, ());
  MOCK_METHOD(ComputeCommandBuffer*, CreateComputeCommandBuffer, ());
  MOCK_METHOD(void, DestroyCommandBuffer, (GraphicsCommandBuffer*));
  MOCK_METHOD(void, DestroyCommandBuffer, (ComputeCommandBuffer*));
};

class FakeRHIFactory final : public RHIFactory {
 public:
  FakeRHIFactory() {
    m_pipelineContext = std::make_unique<MockPipelineContext>();
    m_bufferContext = std::make_unique<MockBufferContext>();
  }
  ~FakeRHIFactory() {}

 public:
  void
  Init(GLFWwindow* window, uint32_t width, uint32_t height) override {}

  void
  Quit() override {}

 public:
  std::vector<SampleCount>
  FindSupportSampleCount() override {
    return {};
  }

 public:
  Swapchain*
  GetSwapchain() override {
    return nullptr;
  }

  Swapchain*
  RecreateSwapchain(Swapchain* oldSwapchain, uint32_t width, uint32_t height) override {
    return nullptr;
  }

  int
  AcquireNextImage(Swapchain* swapchain, const Semaphore* semaphore) override {
    return 1;
  }

  int
  Present(Swapchain* swapchain, std::span<Semaphore*> waitSemaphores, uint32_t imageIndex) override {
    return 1;
  }

  void
  WaitIdle() override {
    return;
  }

 public:
  Fence*
  CreateFence() override {
    return nullptr;
  }

  void
  WaitForFence(Fence* fence) override {
    return;
  }

  void
  ResetFence(Fence* fence) override {
    return;
  }

  MOCK_METHOD(void, DestroyFence, (Fence*), (override));
  MOCK_METHOD(Semaphore*, CreateGPUSemaphore, (), (override));

  void
  DestroyGPUSemaphore(Semaphore* semaphore) override {
    return;
  }
};

}  // namespace Marbas
