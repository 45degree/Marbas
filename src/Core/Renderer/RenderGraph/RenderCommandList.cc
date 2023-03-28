#include "RenderCommandList.hpp"

#include "RenderGraphPass.hpp"

namespace Marbas {

GraphicsRenderCommandList::GraphicsRenderCommandList(details::RenderGraphGraphicsPass* pass, RHIFactory* rhiFactory)
    : m_pass(pass), m_rhiFactory(rhiFactory) {
  const auto& bindings = m_pass->m_pipelineCreateInfo.m_bindings;
  for (const auto& binding : bindings) {
    m_preCountSize[binding.descriptorType]++;
  }
  m_commandPool = m_rhiFactory->GetBufferContext()->CreateCommandPool(CommandBufferUsage::GRAPHICS);
  m_commandBuffer = m_rhiFactory->GetBufferContext()->CreateCommandBuffer(m_commandPool);

  // m_fence = m_rhiFactory->CreateFence();
}

GraphicsRenderCommandList::~GraphicsRenderCommandList() {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  bufCtx->DestroyCommandBuffer(m_commandPool, m_commandBuffer);
  bufCtx->DestroyCommandPool(m_commandPool);

  for (auto* set : m_descriptorSets) {
    pipelineCtx->DestroyDescriptorSet(m_descriptorPool, set);
  }
  m_descriptorSets.clear();
  pipelineCtx->DestroyDescriptorPool(m_descriptorPool);
}

void
GraphicsRenderCommandList::SetDescriptorSetCount(uint32_t count) {
  if (count <= m_descriptorSets.size()) return;

  auto* pipelineCtx = m_rhiFactory->GetPipelineContext();

  for (auto* set : m_descriptorSets) {
    pipelineCtx->DestroyDescriptorSet(m_descriptorPool, set);
  }
  m_descriptorSets.clear();
  pipelineCtx->DestroyDescriptorPool(m_descriptorPool);

  Vector<DescriptorPoolSize> sizes;
  for (const auto& [type, size] : m_preCountSize) {
    DescriptorPoolSize descriptorSize;
    descriptorSize.type = type;
    descriptorSize.size = size * count;
    sizes.push_back(descriptorSize);
  }

  m_descriptorPool = pipelineCtx->CreateDescriptorPool(sizes, count);
  for (int i = 0; i < count; i++) {
    auto* descriptorSet = pipelineCtx->CreateDescriptorSet(m_descriptorPool, m_pass->m_layout);
    m_descriptorSets.push_back(descriptorSet);
  }
}

void
GraphicsRenderCommandList::Begin(const Vector<ClearValue>& clearValue) {
  auto* framebuffer = m_pass->m_framebuffer;
  auto* pipeline = m_pass->m_pipeline;
  std::array<ViewportInfo, 1> viewPortInfo = {
      ViewportInfo{0, 0, static_cast<float>(framebuffer->width), static_cast<float>(framebuffer->height), 0, 1},
  };
  std::array<ScissorInfo, 1> scissorInfo = {
      ScissorInfo{0, 0, framebuffer->width, framebuffer->height},
  };
  m_commandBuffer->BeginPipeline(m_pass->m_pipeline, m_pass->m_framebuffer, clearValue);

  // TODO: add to a method
  m_commandBuffer->SetViewports(viewPortInfo);
  m_commandBuffer->SetScissors(scissorInfo);
  m_currentSetIndex = 0;
}

void
GraphicsRenderCommandList::End() {
  m_commandBuffer->EndPipeline(m_pass->m_pipeline);
  m_currentSetIndex = 0;
}

void
GraphicsRenderCommandList::BindVertexBuffer(Buffer* vertexBuffer) {
  m_commandBuffer->BindVertexBuffer(vertexBuffer);
}

void
GraphicsRenderCommandList::BindIndexBuffer(Buffer* indexBuffer) {
  m_commandBuffer->BindIndexBuffer(indexBuffer);
}

void
GraphicsRenderCommandList::BindArgument(const RenderArgument& argument) {
  auto* pipelineCtx = m_rhiFactory->GetPipelineContext();

  auto* descriptorSet = m_descriptorSets[m_currentSetIndex];
  for (auto& [bindingPoint, buffer] : argument.m_uniformBuffers) {
    pipelineCtx->BindBuffer(BindBufferInfo{
        .descriptorSet = descriptorSet,
        .descriptorType = DescriptorType::UNIFORM_BUFFER,
        .bindingPoint = static_cast<uint16_t>(bindingPoint),
        .buffer = buffer,
        .offset = 0,
        .arrayElement = 0,
    });
  }

  for (auto& [bindingPoint, buffer] : argument.m_stroageBuffer) {
    pipelineCtx->BindBuffer(BindBufferInfo{
        .descriptorSet = descriptorSet,
        .descriptorType = DescriptorType::STORAGE_BUFFER,
        .bindingPoint = static_cast<uint16_t>(bindingPoint),
        .buffer = buffer,
        .offset = 0,
        .arrayElement = 0,
    });
  }

  for (auto& [bindingPoint, imageInfo] : argument.m_images) {
    pipelineCtx->BindImage(BindImageInfo{
        .descriptorSet = descriptorSet,
        .bindingPoint = static_cast<uint16_t>(bindingPoint),
        .imageView = imageInfo.imageView,
        .sampler = imageInfo.sampler,
    });
  }

  m_commandBuffer->BindDescriptorSet(m_pass->m_pipeline, descriptorSet);

  m_currentSetIndex++;
}

void
GraphicsRenderCommandList::Submit(Semaphore* waitSemaphore, Semaphore* signalSemaphore, Fence* fence) {
  std::span<Semaphore*> waitSemaphores;
  std::span<Semaphore*> signalSemaphores;
  if (waitSemaphore == nullptr) {
    waitSemaphores = std::span<Semaphore*>{&waitSemaphore, 0};
  } else {
    waitSemaphores = std::span<Semaphore*>{&waitSemaphore, 1};
  }

  if (signalSemaphore == nullptr) {
    signalSemaphores = std::span<Semaphore*>{&signalSemaphore, 0};
  } else {
    signalSemaphores = std::span<Semaphore*>{&signalSemaphore, 1};
  }

  m_commandBuffer->Submit(waitSemaphores, signalSemaphores, fence);
}

};  // namespace Marbas
