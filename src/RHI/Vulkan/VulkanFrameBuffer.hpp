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
  vk::Framebuffer m_frameBuffer;
};

}  // namespace Marbas
