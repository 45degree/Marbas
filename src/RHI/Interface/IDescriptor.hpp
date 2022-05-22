#pragma once

namespace Marbas {

enum class BufferDescriptorType {
  UNIFORM_BUFFER,
  DYNAMIC_UNIFORM_BUFFER,
};

class IBufferDescriptor {
 protected:
  BufferDescriptorType m_type;

 public:
  explicit IBufferDescriptor(BufferDescriptorType type) : m_type(type) {}
  virtual ~IBufferDescriptor() = default;

 public:
  [[nodiscard]] BufferDescriptorType
  GetDescriptorType() const noexcept {
    return m_type;
  }
};

class IDynamicBufferDescriptor {
 protected:
  BufferDescriptorType m_type = BufferDescriptorType::DYNAMIC_UNIFORM_BUFFER;

 public:
  IDynamicBufferDescriptor() = default;
  virtual ~IDynamicBufferDescriptor() = default;
};

class IImageDescriptor {
 public:
  IImageDescriptor() = default;
  virtual ~IImageDescriptor() = default;
};

}  // namespace Marbas
