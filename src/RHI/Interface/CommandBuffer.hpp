#pragma once

#include <memory>

#include "RHI/Interface/DescriptorSet.hpp"
#include "RHI/Interface/FrameBuffer.hpp"
#include "RHI/Interface/Pipeline.hpp"
#include "RHI/Interface/RenderPass.hpp"
#include "RHI/Interface/VertexBuffer.hpp"
namespace Marbas {

class ICommand;

// bind descriptor set
struct DynamicBufferPiece {
  uint32_t offset = 0;
  uint32_t size = 0;
};
struct BindDescriptorSetInfo {
  std::shared_ptr<DescriptorSet> descriptorSet;
  DescriptorSetLayout layouts;
  Vector<DynamicBufferPiece> bufferPiece;
};

// begin render pass
struct BeginRenderPassInfo {
  std::shared_ptr<RenderPass> renderPass;
  std::shared_ptr<FrameBuffer> frameBuffer;
  std::array<float, 4> clearColor = {0, 0, 0, 0};
};

// copy image info
struct CopyImageToImageInfo {
  std::shared_ptr<Texture> srcTexture;
  std::shared_ptr<Texture> dstTexture;
};

class CommandBuffer {
 public:
  virtual void
  Clear() = 0;

  virtual void
  BeginRecordCmd() = 0;

  virtual void
  BindDescriptorSet(const BindDescriptorSetInfo& info) = 0;

  virtual void
  BeginRenderPass(const BeginRenderPassInfo& info) = 0;

  virtual void
  BindPipeline(const std::shared_ptr<GraphicsPipeLine>& graphicsPipeline) = 0;

  virtual void
  BindVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;

  virtual void
  BindIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

  virtual void
  DrawIndex(int vertexCount, int instanceCount) = 0;

  virtual void
  DrawArray(int vertexCount, int instanceCount) = 0;

  virtual void
  CopyImageToImage(const CopyImageToImageInfo& info) = 0;

  virtual void
  EndRenderPass() = 0;

  virtual void
  EndRecordCmd() = 0;

  virtual void
  SubmitCommand() = 0;
};

}  // namespace Marbas
