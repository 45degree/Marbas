#ifndef MARBARS_RHI_INTERFACE_INDEX_BUFFER_H
#define MARBARS_RHI_INTERFACE_INDEX_BUFFER_H

#include "Common.hpp"

namespace Marbas {

class IndexBuffer {
 public:
  explicit IndexBuffer(size_t count) : m_indexCount(count) {}
  virtual ~IndexBuffer() = default;

 public:
  virtual void Bind() const = 0;
  virtual void UnBind() const = 0;
  virtual void SetData(const Vector<uint32_t>& data, size_t offsetCount) = 0;

  [[nodiscard]] size_t GetIndexCount() const noexcept { return m_indexCount; }

 protected:
  size_t m_indexCount = 0;
};

}  // namespace Marbas

#endif
