#pragma once

#include "Common/Common.hpp"
#include "RHI/Interface/VertexBuffer.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

class OpenGLVertexBuffer : public VertexBuffer {
 public:
  explicit OpenGLVertexBuffer(uint32_t size);
  explicit OpenGLVertexBuffer(const void* data, uint32_t size);

  ~OpenGLVertexBuffer() override;

 public:
  void
  Bind() const override;

  void
  UnBind() const override;

  void
  SetData(const void* data, uint32_t size, uint32_t offset) override;

 private:
  GLuint VBO;
};

}  // namespace Marbas
