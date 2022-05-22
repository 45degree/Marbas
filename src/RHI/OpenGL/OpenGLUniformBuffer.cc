#include "RHI/OpenGL/OpenGLUniformBuffer.hpp"

#include <glog/logging.h>

#include <memory>

namespace Marbas {

OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size) : UniformBuffer(size) {
  glCreateBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glNamedBufferData(UBO, m_size, nullptr, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  m_bufferDescriptor = std::make_shared<OpenGLUniformBufferDescriptor>(UBO);

  LOG(INFO) << "create uniform buffer";
}

OpenGLUniformBuffer::~OpenGLUniformBuffer() {
  glDeleteBuffers(1, &UBO);
  LOG(INFO) << "delete uniform buffer";
}

void
OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset) {
  glNamedBufferSubData(UBO, offset, size, data);
}

OpenGLDynamicUniformBuffer::OpenGLDynamicUniformBuffer(uint32_t size) : DynamicUniformBuffer(size) {
  glCreateBuffers(1, &m_ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
  glNamedBufferData(m_ubo, m_size, nullptr, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  m_dynamicBufferDescriptor = std::make_shared<OpenGLDynamicUniformBufferDescriptor>(m_ubo);

  LOG(INFO) << "create dynamic uniform buffer";
}

OpenGLDynamicUniformBuffer::~OpenGLDynamicUniformBuffer() {
  glDeleteBuffers(1, &m_ubo);
  LOG(INFO) << "delete dynamic uniform buffer";
}

void
OpenGLDynamicUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset) {
  glNamedBufferSubData(m_ubo, offset, size, data);
}

}  // namespace Marbas
