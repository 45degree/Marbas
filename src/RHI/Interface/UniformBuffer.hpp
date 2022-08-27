#pragma once

#include <cstdint>
#include <memory>

#include "RHI/Interface/IBuffer.hpp"
#include "RHI/Interface/IDescriptor.hpp"

namespace Marbas {

class UniformBuffer : public IBuffer {
 public:
  explicit UniformBuffer(uint32_t size) : m_size(size) {}
  virtual ~UniformBuffer() = default;

 public:
  virtual BufferDescriptorType
  GetBufferDescriptorType() const {
    return BufferDescriptorType::UNIFORM_BUFFER;
  }

 protected:
  uint32_t m_size;
};

class DynamicUniformBuffer : public IBuffer {
 public:
  explicit DynamicUniformBuffer(uint32_t size) : m_size(size) {}
  virtual ~DynamicUniformBuffer() = default;

 public:
  // virtual std::shared_ptr<IDynamicBufferDescriptor>
  // GetIDynamicBufferDescriptor() const = 0;
  virtual BufferDescriptorType
  GetBufferDescriptorType() const {
    return BufferDescriptorType::DYNAMIC_UNIFORM_BUFFER;
  }

 protected:
  uint32_t m_size;
};

}  // namespace Marbas
