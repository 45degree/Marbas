#pragma once

#include "RHI/Interface/RenderPass.hpp"
namespace Marbas {

struct FrameBufferInfo {
  uint32_t width = 0;
  uint32_t height = 0;

  // if the renderpass is null, means the frame buffer is a default frame buffer
  const RenderPass* renderPass = nullptr;
  Vector<std::shared_ptr<Texture2D>> attachments;
};

class FrameBuffer {
 public:
  explicit FrameBuffer(const FrameBufferInfo& info) : m_width(info.width), m_height(info.height){};
  virtual ~FrameBuffer() = default;

 public:
  virtual void
  Bind() const = 0;

  virtual void
  UnBind() const = 0;

 protected:
  uint32_t m_width;
  uint32_t m_height;
};

}  // namespace Marbas
