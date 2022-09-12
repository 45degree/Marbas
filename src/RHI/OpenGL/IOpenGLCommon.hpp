#pragma once

#include <cstdint>

#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

class IOpenGLBindable {
 public:
  virtual void
  Bind(uint16_t bindingPoint) const = 0;

  virtual void
  UnBind() const = 0;
};

class IOpenGLDynamicBindable {
 public:
  virtual void
  Bind(uint16_t bindingPoint, uint32_t offset, uint32_t size) const = 0;

  virtual void
  UnBind() const = 0;
};

class IOpenGLTaget {
 public:
  virtual GLuint
  GetOpenGLTarget() const = 0;
};

}  // namespace Marbas
