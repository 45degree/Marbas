#pragma once

#include <memory>

#include "RHI/Interface/FrameBuffer.hpp"

namespace Marbas {

class SwapChain {
 public:
  SwapChain(uint32_t width, uint32_t height) : m_width(width), m_height(height) {}

 public:

  virtual std::shared_ptr<FrameBuffer>
  GetDefaultFrameBuffer() = 0;

  virtual void
  Present() = 0;

 protected:
  uint32_t m_width;
  uint32_t m_height;
};

}  // namespace Marbas
