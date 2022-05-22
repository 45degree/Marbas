#pragma once

#include "Common/Common.hpp"
#include "RHI/Interface/UniformBuffer.hpp"
#include "RHI/OpenGL/IOpenGLDescriptor.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

class OpenGLUniformBufferDescriptor final : public IOpenGLBufferDescriptor {
 public:
  explicit OpenGLUniformBufferDescriptor(GLuint target)
      : IOpenGLBufferDescriptor(BufferDescriptorType::UNIFORM_BUFFER), m_target(target) {}
  ~OpenGLUniformBufferDescriptor() override = default;

 public:
  void
  Bind(uint16_t bindingPoint) const override {
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_target);
  }

  void
  UnBind() const override {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

 private:
  GLuint m_target;
};

class OpenGLUniformBuffer : public UniformBuffer {
 public:
  explicit OpenGLUniformBuffer(uint32_t size);
  ~OpenGLUniformBuffer() override;

 public:
  void
  SetData(const void* data, uint32_t size, uint32_t offset) override;

  std::shared_ptr<IBufferDescriptor>
  GetIBufferDescriptor() const override {
    return m_bufferDescriptor;
  }

 private:
  GLuint UBO;
  std::shared_ptr<IOpenGLBufferDescriptor> m_bufferDescriptor = nullptr;
};

class OpenGLDynamicUniformBufferDescriptor final : public IOpenGLDynamicBufferDescriptor {
 public:
  explicit OpenGLDynamicUniformBufferDescriptor(GLuint target)
      : IOpenGLDynamicBufferDescriptor(), m_target(target) {}
  ~OpenGLDynamicUniformBufferDescriptor() override = default;

 public:
  void
  Bind(uint16_t bindingPoint, uint32_t offset, uint32_t size) const override {
    glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, m_target, offset, size);
  }

  void
  UnBind() const override {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

 private:
  GLuint m_target;
};

class OpenGLDynamicUniformBuffer final : public DynamicUniformBuffer {
 public:
  explicit OpenGLDynamicUniformBuffer(uint32_t size);
  ~OpenGLDynamicUniformBuffer() override;

 public:
  void
  SetData(const void* data, uint32_t size, uint32_t offset) override;

  std::shared_ptr<IDynamicBufferDescriptor>
  GetIDynamicBufferDescriptor() const override {
    return m_dynamicBufferDescriptor;
  }

 private:
  GLuint m_ubo;
  std::shared_ptr<IOpenGLDynamicBufferDescriptor> m_dynamicBufferDescriptor = nullptr;
};

}  // namespace Marbas
