#pragma once

#include <cstdint>

#include "RHI/Interface/IDescriptor.hpp"

namespace Marbas {

class IOpenGLBufferDescriptor : public IBufferDescriptor {
 public:
  IOpenGLBufferDescriptor(BufferDescriptorType type) : IBufferDescriptor(type) {}
  ~IOpenGLBufferDescriptor() override = default;

 public:
  virtual void
  Bind(uint16_t bindingPoint) const = 0;

  virtual void
  UnBind() const = 0;
};

class IOpenGLDynamicBufferDescriptor : public IDynamicBufferDescriptor {
 public:
  IOpenGLDynamicBufferDescriptor() : IDynamicBufferDescriptor() {}
  ~IOpenGLDynamicBufferDescriptor() override = default;

 public:
  virtual void
  Bind(uint16_t bindingPoint, uint32_t offset, uint32_t size) const = 0;

  virtual void
  UnBind() const = 0;
};

class IOpenGLImageDescriptor : public IImageDescriptor {
 public:
  IOpenGLImageDescriptor() : IImageDescriptor() {}
  ~IOpenGLImageDescriptor() override = default;

 public:
  virtual void
  Bind(uint16_t bindingPoint) const = 0;
  virtual void
  UnBind() const = 0;
};

}  // namespace Marbas
