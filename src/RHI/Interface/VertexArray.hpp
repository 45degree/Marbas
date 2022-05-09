#ifndef MARBARS_RHI_INTERFACE_VERTEX_ARRAY_H
#define MARBARS_RHI_INTERFACE_VERTEX_ARRAY_H

#include "Common.hpp"
#include "RHI/Interface/VertexBuffer.hpp"

namespace Marbas {

class VertexArray {
 public:
  VertexArray() = default;
  virtual ~VertexArray() = default;

 public:
  virtual void Bind() const = 0;
  virtual void UnBind() const = 0;

  virtual void EnableVertexAttribArray(const VertexBuffer* vertexBuffer) const = 0;
};

}  // namespace Marbas

#endif
