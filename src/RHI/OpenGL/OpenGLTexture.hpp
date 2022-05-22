#pragma once

#include "RHI/Interface/Texture.hpp"
#include "RHI/OpenGL/IOpenGLDescriptor.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

class OpenGLTexture2DDescriptor : public IOpenGLImageDescriptor {
 public:
  explicit OpenGLTexture2DDescriptor(GLuint target) : IOpenGLImageDescriptor(), m_target(target) {}

 public:
  void
  Bind(uint16_t bindingPoint) const override {
    glBindTextureUnit(bindingPoint, m_target);
  }
  void
  UnBind() const override {
    glBindTexture(GL_TEXTURE_2D, 0);
  }

 private:
  GLuint m_target;
};

class OpenGLTextureCubeMapDescriptor final : public IOpenGLImageDescriptor {
 public:
  explicit OpenGLTextureCubeMapDescriptor(GLuint target)
      : IOpenGLImageDescriptor(), m_target(target) {}

 public:
  void
  Bind(uint16_t bindingPoint) const override {
    glBindTextureUnit(bindingPoint, m_target);
  }
  void
  UnBind() const override {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  }

 private:
  GLuint m_target;
};

class OpenGLTexture2D : public Texture2D {
 public:
  OpenGLTexture2D(int width, int height, TextureFormat format);
  ~OpenGLTexture2D() override;

 public:
  void
  SetData(void* data, uint32_t size) override;

  void*
  GetTexture() override;

  GLuint
  GetOpenGLTexture() const noexcept {
    return textureID;
  }

  std::shared_ptr<IImageDescriptor>
  GetDescriptor() const override {
    return m_descriptor;
  }

 private:
  GLuint textureID = 0;
  std::shared_ptr<OpenGLTexture2DDescriptor> m_descriptor = nullptr;
};

class OpenGLTextureCubeMap : public TextureCubeMap {
 public:
  explicit OpenGLTextureCubeMap(int width, int height, TextureFormat format);

  ~OpenGLTextureCubeMap() override = default;

 public:
  void
  SetData(void* data, uint32_t size, CubeMapPosition position) override;

  std::shared_ptr<IImageDescriptor>
  GetDescriptor() const override {
    return m_descriptor;
  }

 private:
  std::shared_ptr<OpenGLTextureCubeMapDescriptor> m_descriptor = nullptr;
  GLuint m_textureID;
};

}  // namespace Marbas
