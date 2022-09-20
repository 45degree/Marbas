#pragma once

#include "RHI/Interface/Texture.hpp"
#include "RHI/OpenGL/IOpenGLCommon.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

GLenum
ConvertToOpenglDataFormat(TextureFormat type);

GLenum
ConvertToOpenGLTextureType(TextureType type);

GLenum
ConvertToOpenGLTextureDataType(TextureFormat type);

GLenum
ConvertToOpenglInternalFormat(TextureFormat type);

class OpenGLTexture;
struct OpenGLImageView final : public ImageView {
  GLuint m_textureTarget;
  GLenum m_openGLFormat;
  GLenum m_openGLType;

  virtual ~OpenGLImageView() { glDeleteTextures(1, &m_textureTarget); }
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
