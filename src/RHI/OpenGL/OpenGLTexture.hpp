#pragma once

#include "RHI/Interface/Texture.hpp"
#include "RHI/OpenGL/IOpenGLCommon.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

class OpenGLTexture;
struct OpenGLImageView final : public ImageView {
  std::shared_ptr<OpenGLTexture> texture;
  uint32_t level = 0;
  uint32_t layer = 0;

  void
  SetTexture(const std::shared_ptr<Texture>& tex) override {
    texture = std::static_pointer_cast<OpenGLTexture>(tex);
  }

  void
  SetRangeInfo(uint32_t layerBase, uint32_t layerCount, uint32_t levelBase,
               uint32_t levelCount) override {
    level = levelBase;
    layer = layerBase;
  }
};

class OpenGLTexture final : public Texture {
 public:
  explicit OpenGLTexture(const ImageDesc& desc);
  virtual ~OpenGLTexture() = default;

 public:
  void
  SetData(void* data, size_t size, uint32_t level = 0, uint32_t layer = 0) override;

 public:
  void*
  GetOriginHandle() override {
    return reinterpret_cast<void*>(m_target);
  }

  GLuint
  GetOpenGLTarget() const {
    return m_target;
  }

  void
  Bind(uint16_t bindingPoint) const noexcept {
    glBindTextureUnit(bindingPoint, m_target);
    m_bindingPoint = bindingPoint;
  };

  void
  UnBind() const noexcept;

  GLenum
  GetOpenGLType() const;

  GLenum
  GetOpenGLFormat() const;

 protected:
  GLuint m_target;
  mutable std::optional<uint16_t> m_bindingPoint = 0;
};

}  // namespace Marbas
