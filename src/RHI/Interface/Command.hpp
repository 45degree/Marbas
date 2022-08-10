#pragma once

#include "RHI/Interface/CommandBuffer.hpp"
#include "RHI/Interface/DescriptorSet.hpp"
#include "RHI/Interface/FrameBuffer.hpp"
#include "RHI/Interface/IndexBuffer.hpp"
#include "RHI/Interface/Pipeline.hpp"
#include "RHI/Interface/RenderPass.hpp"
#include "RHI/Interface/VertexBuffer.hpp"

namespace Marbas {

class ICommand {
 public:
  virtual ~ICommand() = default;

 public:
  virtual void
  Execute() const = 0;

  virtual std::unique_ptr<ICommand>
  Clone() const = 0;
};

class BindDescriptorSet : public ICommand {
 public:
  virtual ~BindDescriptorSet() = default;

 public:
  virtual void
  SetDescriptor(const std::shared_ptr<DescriptorSet>& descriptorSet) = 0;
};

class BindDynamicDescriptorSet : public ICommand {
 public:
  virtual ~BindDynamicDescriptorSet() = default;

 public:
  virtual void
  SetDescriptorSet(const std::shared_ptr<DynamicDescriptorSet>& descriptorSet) = 0;

  void
  SetOffset(uint32_t offset) {
    m_offset = offset;
  }

  void
  SetSize(uint32_t size) {
    m_size = size;
  }

 protected:
  uint32_t m_offset = 0;
  uint32_t m_size = 0;
};

class BeginRenderPass : public ICommand {
 public:
  virtual ~BeginRenderPass() = default;

 public:
  virtual void
  SetRenderPass(const std::shared_ptr<RenderPass>& renderPass) = 0;

  virtual void
  SetFrameBuffer(const std::shared_ptr<FrameBuffer>& frameBuffer) = 0;

  virtual void
  SetClearColor(const std::array<float, 4>& clearColor) = 0;
};

class EndRenderPass : public ICommand {
 public:
  virtual ~EndRenderPass() = default;

 public:
  virtual void
  SetRenderPass(const std::shared_ptr<RenderPass>& renderPass) = 0;

  virtual void
  SetFrameBuffer(const std::shared_ptr<FrameBuffer>& frameBuffer) = 0;
};

class BindPipeline : public ICommand {
 public:
  virtual ~BindPipeline() = default;

 public:
  virtual void
  SetPipeLine(const std::shared_ptr<GraphicsPipeLine>& graphicsPipeline) = 0;
};

class BindVertexBuffer : public ICommand {
 public:
  virtual ~BindVertexBuffer() = default;

 public:
  virtual void
  SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
};

class BindIndexBuffer : public ICommand {
 public:
  virtual ~BindIndexBuffer() = default;

 public:
  virtual void
  SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;
};

class DrawIndex : public ICommand {
 public:
  virtual ~DrawIndex() = default;

 public:
  virtual void
  SetIndexCount(int count) = 0;

  virtual void
  SetInstanceCount(int count) = 0;
};

class DrawArray : public ICommand {
 public:
  virtual ~DrawArray() = default;

 public:
  virtual void
  SetVertexCount(int count) = 0;

  virtual void
  SetInstanceCount(int count) = 0;
};

class CopyImageToImage : public ICommand {
 public:
  virtual ~CopyImageToImage() = default;

 public:
  virtual void
  SetSrcImage(const std::shared_ptr<Texture2D>& srcTexture);

  virtual void
  SetDstImage(const std::shared_ptr<Texture2D>& dstTexture);
};

class CopyImageToFrameBuffer : public ICommand {
 public:
  virtual ~CopyImageToFrameBuffer() = default;

 public:
  virtual void
  SetImage(const std::shared_ptr<Texture2D>& texture) = 0;

  virtual void
  SetFrameBuffer(const std::shared_ptr<FrameBuffer>& frameBuffer) = 0;

  virtual void
  ChooseAttachment(AttachmentType attachmentType, uint32_t index) = 0;
};

}  // namespace Marbas
