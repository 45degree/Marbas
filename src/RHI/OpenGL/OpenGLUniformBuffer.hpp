#pragma once

#include "Common/Common.hpp"
#include "RHI/Interface/UniformBuffer.hpp"
#include "RHI/OpenGL/IOpenGLCommon.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

class OpenGLUniformBuffer final : public UniformBuffer, IOpenGLBindable {
 public:
  explicit OpenGLUniformBuffer(uint32_t size);
  ~OpenGLUniformBuffer() override;

 public:
  void
  SetData(const void* data, uint32_t size, uint32_t offset) override;

  void
  Bind(uint16_t bindingPoint) const noexcept override {
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_target);
    m_bindingPoint = bindingPoint;
  }

  void
  UnBind() const noexcept override {
    if (m_bindingPoint.has_value()) {
      glBindBufferBase(GL_UNIFORM_BUFFER, *m_bindingPoint, 0);
    }
  }

 private:
  GLuint m_target;
  mutable std::optional<uint16_t> m_bindingPoint;
};

class OpenGLDynamicUniformBuffer final : public DynamicUniformBuffer, IOpenGLDynamicBindable {
 public:
  explicit OpenGLDynamicUniformBuffer(uint32_t size);
  virtual ~OpenGLDynamicUniformBuffer();

 public:
  void
  SetData(const void* data, uint32_t size, uint32_t offset) override;

  void
  Bind(uint16_t bindingPoint, uint32_t offset, uint32_t size) const noexcept override {
    glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, m_target, offset, size);
    m_bindingPoint = bindingPoint;
  }

  void
  UnBind() const noexcept override {
    if (m_bindingPoint.has_value()) {
      glBindBufferBase(GL_UNIFORM_BUFFER, *m_bindingPoint, 0);
    }
  }

 private:
  GLuint m_target;
  mutable std::optional<uint16_t> m_bindingPoint;
};

}  // namespace Marbas
