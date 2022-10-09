#pragma once

#include <memory>

#include "RHI/Interface/FrameBuffer.hpp"
#include "RHI/Interface/Semaphore.hpp"

namespace Marbas {

class SwapChain {
 public:
  SwapChain(uint32_t width, uint32_t height) : m_width(width), m_height(height) {}

 public:
  // FIX: need to remove
  virtual std::shared_ptr<FrameBuffer>
  GetDefaultFrameBuffer() = 0;

  virtual size_t
  GetImageCount() const = 0;

  virtual int
  AcquireNextImage(const Semaphore& semaphore) = 0;

  virtual void
  Resize(uint32_t width, uint32_t height) = 0;

  virtual int
  Present(const Vector<Semaphore>& waitSemaphores, uint32_t imageIndex) = 0;

  uint32_t
  GetWidth() const noexcept {
    return m_width;
  }

  uint32_t
  GetHeight() const noexcept {
    return m_height;
  }

 protected:
  uint32_t m_width;
  uint32_t m_height;
};

}  // namespace Marbas
