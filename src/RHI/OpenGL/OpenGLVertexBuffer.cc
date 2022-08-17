#include "RHI/OpenGL/OpenGLVertexBuffer.hpp"

#include <glog/logging.h>

namespace Marbas {

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) : VertexBuffer(size), VBO(0) {
  LOG(INFO) << "create an opengl vertex buffer";

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  LOG(INFO) << "opengl vertex buffer create finish";
}

OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, uint32_t size)
    : VertexBuffer(size), VBO(0) {
  LOG(INFO) << "create an opengl vertex buffer";

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  LOG(INFO) << "opengl vertex buffer create finish";
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
  LOG(INFO) << "delete vertex buffer";
  glDeleteBuffers(1, &VBO);
}

void
OpenGLVertexBuffer::Bind() const {
  // glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindVertexBuffer(0, VBO, 0, m_stride);
}

void
OpenGLVertexBuffer::UnBind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
OpenGLVertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset) {
  LOG(INFO) << FORMAT("set vertex buffer data, the buffer size is {}, offset is {}", m_size,
                      offset);

  if (offset + size > m_size) {
    LOG(ERROR) << FORMAT("can't set data for buffer, this buffer's max size is {}", m_size);
    return;
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(offset), static_cast<GLsizeiptr>(size),
                  data);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}  // namespace Marbas
