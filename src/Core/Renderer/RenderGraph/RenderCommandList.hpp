#pragma once

#include <cstdint>

#include "Common/Common.hpp"
#include "RHIFactory.hpp"

namespace Marbas::details {
class RenderGraphGraphicsPass;
};

namespace Marbas {

class RenderArgument {
  friend class GraphicsRenderCommandList;

  struct BindedImageInfo {
    ImageView* imageView;
    Sampler* sampler;
  };

 public:
  void
  BindUniformBuffer(uint32_t bindingPoint, Buffer* buffer) {
    m_uniformBuffers[bindingPoint] = buffer;
  }

  void
  BindStorageBuffer(uint32_t bindingPoint, Buffer* buffer) {
    m_stroageBuffer[bindingPoint] = buffer;
  }

  void
  BindImage(uint32_t bindingPoint, Sampler* sampler, ImageView* imageView) {
    m_images[bindingPoint] = {imageView, sampler};
  }

 private:
  HashMap<uint32_t, Buffer*> m_stroageBuffer;
  HashMap<uint32_t, Buffer*> m_uniformBuffers;
  HashMap<uint32_t, BindedImageInfo> m_images;
};

class GraphicsRenderCommandList final {
 public:
  GraphicsRenderCommandList(details::RenderGraphGraphicsPass* pass, RHIFactory* rhiFactory);
  ~GraphicsRenderCommandList();

 public:
  void
  Begin(const Vector<ClearValue>& clearValue);

  void
  End();

  void
  SetDescriptorSetCount(uint32_t count);

  void
  BindArgument(const RenderArgument& argument);

  void
  BindVertexBuffer(Buffer* vertexBuffer);

  void
  BindIndexBuffer(Buffer* indexBuffer);

  void
  DrawIndex(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset,
            uint32_t firstInstance) {
    m_commandBuffer->DrawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstIndex);
  }

  void
  Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    m_commandBuffer->Draw(vertexCount, instanceCount, firstVertex, firstInstance);
  }

  void
  Submit(Semaphore* waitSemaphore, Semaphore* signalSemaphore, Fence* fence);

  void
  BeginRecord() {
    m_commandBuffer->Begin();
  }

  void
  EndRecord() {
    m_commandBuffer->End();
  }

 private:
  // Fence* m_fence;
  uint32_t m_currentSetIndex = 0;
  RHIFactory* m_rhiFactory = nullptr;
  DescriptorPool* m_descriptorPool = nullptr;
  Vector<DescriptorSet*> m_descriptorSets;
  details::RenderGraphGraphicsPass* m_pass = nullptr;
  CommandBuffer* m_commandBuffer = nullptr;
  CommandPool* m_commandPool = nullptr;
  HashMap<DescriptorType, uint32_t> m_preCountSize;
};

}  // namespace Marbas
