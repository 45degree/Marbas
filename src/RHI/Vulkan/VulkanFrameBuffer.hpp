#pragma once

#include <vulkan/vulkan.hpp>

#include "RHI/Interface/FrameBuffer.hpp"

namespace Marbas {

class VulkanFrameBuffer final : public FrameBuffer {
 public:
  explicit VulkanFrameBuffer(const FrameBufferInfo& info);
  virtual ~VulkanFrameBuffer() = default;

 public:
  void
  Bind() const override {}

  void
  UnBind() const override {}

 private:
  Vector<vk::ImageView> m_attachments;
  vk::Framebuffer m_frameBuffer;
  vk::Semaphore m_semaphore;
};

}  // namespace Marbas