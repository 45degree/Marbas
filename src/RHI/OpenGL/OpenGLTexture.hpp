#pragma once

#include "RHI/Interface/Texture.hpp"
#include "RHI/OpenGL/IOpenGLCommon.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

class OpenGLTexture2D : public Texture2D, public IOpenGLBindable {
 public:
  OpenGLTexture2D(int width, int height, uint32_t level, TextureFormat format);
  ~OpenGLTexture2D() override;

 public:
  void
  SetData(void* data, uint32_t size) override;

  void*
  GetTexture() override;

  GLuint
  GetOpenGLTexture() const noexcept {
    return m_textureID;
  }

  void
  Bind(uint16_t bindingPoint) const noexcept override {
    glBindTextureUnit(bindingPoint, m_textureID);
  };

  void
  UnBind() const noexcept override {
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  GLenum
  GetOpenGLFormat() const noexcept;

 private:
  GLuint m_textureID = 0;
};

class OpenGLTextureCubeMap : public TextureCubeMap, public IOpenGLBindable {
 public:
  explicit OpenGLTextureCubeMap(int width, int height, TextureFormat format);

  ~OpenGLTextureCubeMap() override = default;

 public:
  void
  SetData(void* data, uint32_t size, CubeMapPosition position) override;

  void
  Bind(uint16_t bindingPoint) const noexcept override {
    glBindTextureUnit(bindingPoint, m_textureID);
  }

  void
  UnBind() const noexcept override {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  }

  GLuint
  GetOpenGLTexture() const noexcept {
    return m_textureID;
  }

 private:
  GLuint m_textureID;
};

}  // namespace Marbas
