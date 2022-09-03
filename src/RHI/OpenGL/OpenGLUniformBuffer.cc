#include "RHI/OpenGL/OpenGLUniformBuffer.hpp"

#include <glog/logging.h>

#include <memory>

namespace Marbas {

OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size) : UniformBuffer(size) {
  glCreateBuffers(1, &m_target);
  glBindBuffer(GL_UNIFORM_BUFFER, m_target);
  glNamedBufferData(m_target, m_size, nullptr, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  LOG(INFO) << "create uniform buffer";
}

OpenGLUniformBuffer::~OpenGLUniformBuffer() {
  glDeleteBuffers(1, &m_target);
  LOG(INFO) << "delete uniform buffer";
}

void
OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset) {
  glNamedBufferSubData(m_target, offset, size, data);
}

OpenGLDynamicUniformBuffer::OpenGLDynamicUniformBuffer(uint32_t size) : DynamicUniformBuffer(size) {
  glCreateBuffers(1, &m_target);
  glBindBuffer(GL_UNIFORM_BUFFER, m_target);
  glNamedBufferData(m_target, m_size, nullptr, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  LOG(INFO) << "create dynamic uniform buffer";
}

OpenGLDynamicUniformBuffer::~OpenGLDynamicUniformBuffer() {
  glDeleteBuffers(1, &m_target);
  LOG(INFO) << "delete dynamic uniform buffer";
}

void
OpenGLDynamicUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset) {
  glNamedBufferSubData(m_target, offset, size, data);
}

}  // namespace Marbas
