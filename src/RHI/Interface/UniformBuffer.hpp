#ifndef MARBAS_RHI_INTERFACE_UNIFORMBUFFER_H
#define MARBAS_RHI_INTERFACE_UNIFORMBUFFER_H

#include <cstdint>

namespace Marbas {

class UniformBuffer {
 public:
  explicit UniformBuffer(uint32_t size, uint32_t bindingPoint)
      : m_size(size), m_bindingPoint(bindingPoint) {}
  virtual ~UniformBuffer() = default;

 public:
  virtual void Bind() const = 0;

  virtual void UnBind() const = 0;

  virtual void SetData(const void* data, uint32_t size, uint32_t offset) = 0;

 protected:
  uint32_t m_size;
  uint32_t m_bindingPoint;
};

}  // namespace Marbas

#endif
