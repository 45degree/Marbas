#include "RHI/OpenGL/OpenGLTexture.hpp"

#include <glog/logging.h>

namespace Marbas {

static GLenum ConvertToOpenglDataFormat(TextureFormatType type) {
  switch (type) {
    case TextureFormatType::BGR:
      return GL_BGR;
    case TextureFormatType::BGRA:
      return GL_BGRA;
    case TextureFormatType::RED:
      return GL_RED;
    case TextureFormatType::RG:
      return GL_RG;
    case TextureFormatType::RGB:
      return GL_RGB;
    case TextureFormatType::RGBA:
      return GL_RGBA;
  }
}

static GLenum ConvertToOpenglInternalFormat(TextureFormatType type) {
  switch (type) {
    case TextureFormatType::BGR:
      return GL_RGB8;
    case TextureFormatType::BGRA:
      return GL_RGBA8;
    case TextureFormatType::RED:
      return GL_R8;
    case TextureFormatType::RG:
      return GL_RG8;
    case TextureFormatType::RGB:
      return GL_RGB8;
    case TextureFormatType::RGBA:
      return GL_RGBA8;
  }
}

OpenGLTexture2D::OpenGLTexture2D(int width, int height, TextureFormatType format)
    : Texture2D(width, height, format) {
  auto internalFormat = ConvertToOpenglInternalFormat(format);

  glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
  glTextureStorage2D(textureID, 1, internalFormat, width, height);

  glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

OpenGLTexture2D::~OpenGLTexture2D() { glDeleteTextures(1, &textureID); }

void OpenGLTexture2D::Bind(int uniformBind) {
  glBindTextureUnit(uniformBind, textureID);

  auto error = glGetError();
  LOG_IF(ERROR, error) << FORMAT("can't bind texture {} to {}, error code is {}", textureID,
                                 uniformBind, error);
}

void OpenGLTexture2D::SetData(void* data, uint32_t size) {
  auto bpp = this->format == TextureFormatType::RGBA ? 4 : 3;
  LOG_IF(ERROR, size != width * height * bpp) << "size and texture size do not match";

  auto dataFormat = ConvertToOpenglDataFormat(this->format);
  glTextureSubImage2D(textureID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

  auto error = glGetError();
  LOG_IF(ERROR, error) << FORMAT("can't submit the data for texture {}, error code is {}",
                                 textureID, error);
}

void OpenGLTexture2D::UnBind() {
  glBindTexture(GL_TEXTURE_2D, 0);

  auto error = glGetError();
  LOG_IF(ERROR, error) << FORMAT("can't unbind texture: {}, error code is {}", textureID, error);
}

void* OpenGLTexture2D::GetTexture() { return reinterpret_cast<void*>(textureID); }

OpenGLTextureCubeMap::OpenGLTextureCubeMap(int width, int height, TextureFormatType format)
    : TextureCubeMap(width, height, format) {
  auto internalFormat = ConvertToOpenglInternalFormat(format);

  glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_textureID);
  glTextureStorage2D(m_textureID, 1, internalFormat, m_width, m_height);

  glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void OpenGLTextureCubeMap::SetData(void* data, uint32_t size, CubeMapPosition position) {
  auto bpp = m_format == TextureFormatType::RGBA ? 4 : 3;
  LOG_IF(ERROR, size != m_width * m_height * bpp) << "size and texture size do not match";

  auto dataFormat = ConvertToOpenglDataFormat(m_format);

  // see wiki https://www.khronos.org/opengl/wiki/Cubemap_Texture
  int layer = 0;
  switch (position) {
    case CubeMapPosition::BACK:
      layer = 4;
      break;  // GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    case CubeMapPosition::FRONT:
      layer = 5;
      break;  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    case CubeMapPosition::TOP:
      layer = 2;
      break;
    case CubeMapPosition::BOTTOM:
      layer = 3;
      break;
    case CubeMapPosition::LEFT:
      layer = 0;
      break;
    case CubeMapPosition::RIGHT:
      layer = 1;
      break;
  }

  glTextureSubImage3D(m_textureID, 0, 0, 0, layer, m_width, m_height, 1, dataFormat,
                      GL_UNSIGNED_BYTE, data);

  auto error = glGetError();
  LOG_IF(ERROR, error) << FORMAT("can't submit the data for cubemap texture {}, error code is {}",
                                 m_textureID, error);
}

void OpenGLTextureCubeMap::Bind(int bindingPoint) {
  glBindTextureUnit(bindingPoint, m_textureID);

  auto error = glGetError();
  LOG_IF(ERROR, error) << FORMAT("can't bind texture {} to {}, error code is {}", m_textureID,
                                 bindingPoint, error);
}

void OpenGLTextureCubeMap::UnBind() {
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  auto error = glGetError();
  LOG_IF(ERROR, error) << FORMAT("can't unbind texture: {}, error code is {}", m_textureID, error);
}

}  // namespace Marbas
