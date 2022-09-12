#include "RHI/OpenGL/OpenGLTexture.hpp"

#include <glog/logging.h>

namespace Marbas {

static GLenum
ConvertToOpenglDataFormat(TextureFormat type) {
  switch (type) {
    case TextureFormat::BGR:
      return GL_BGR;
    case TextureFormat::BGRA:
      return GL_BGRA;
    case TextureFormat::RED:
      return GL_RED;
    case TextureFormat::R32:
      return GL_RED;
    case TextureFormat::RG:
      return GL_RG;
    case TextureFormat::RGB:
      return GL_RGB;
    case TextureFormat::RGBA:
      return GL_RGBA;
    case TextureFormat::RGB16F:
      return GL_RGB;
    case TextureFormat::RGB32F:
      return GL_RGB;
    case TextureFormat::DEPTH:
      return GL_DEPTH_COMPONENT;
  }
}

static constexpr GLenum
ConvertToOpenGLTextureType(TextureFormat type) {
  switch (type) {
    case TextureFormat::BGR:
      return GL_UNSIGNED_BYTE;
    case TextureFormat::BGRA:
      return GL_UNSIGNED_BYTE;
    case TextureFormat::RED:
      return GL_UNSIGNED_BYTE;
    case TextureFormat::R32:
      return GL_FLOAT;
    case TextureFormat::RG:
      return GL_UNSIGNED_BYTE;
    case TextureFormat::RGB:
      return GL_UNSIGNED_BYTE;
    case TextureFormat::RGBA:
      return GL_UNSIGNED_BYTE;
    case TextureFormat::RGB16F:
      return GL_FLOAT;
    case TextureFormat::RGB32F:
      return GL_FLOAT;
    case TextureFormat::DEPTH:
      return GL_FLOAT;
  }
  return GL_UNSIGNED_BYTE;
}

static GLenum
ConvertToOpenglInternalFormat(TextureFormat type) {
  switch (type) {
    case TextureFormat::BGR:
      return GL_RGB8;
    case TextureFormat::BGRA:
      return GL_RGBA8;
    case TextureFormat::RED:
      return GL_R8;
    case TextureFormat::R32:
      return GL_R32F;
    case TextureFormat::RG:
      return GL_RG8;
    case TextureFormat::RGB:
      return GL_RGB8;
    case TextureFormat::RGBA:
      return GL_RGBA8;
    case TextureFormat::RGB16F:
      return GL_RGB16F;
    case TextureFormat::RGB32F:
      return GL_RGB32F;
    case TextureFormat::DEPTH:
      return GL_DEPTH_COMPONENT32;
  }
}

/**
 * Opengl Texture2D
 */

OpenGLTexture::OpenGLTexture(const ImageDesc& desc) : Texture(desc) {
  auto internalFormat = ConvertToOpenglInternalFormat(desc.format);
  auto width = m_imageDesc.width;
  auto height = m_imageDesc.height;
  auto level = m_imageDesc.mipmapLevel;
  auto depth = m_imageDesc.arrayLayer;
  switch (m_imageDesc.textureType) {
    case TextureType::TEXTURE2D:
      glCreateTextures(GL_TEXTURE_2D, 1, &m_target);
      glTextureStorage2D(m_target, level, internalFormat, width, height);
      break;
    case TextureType::CUBEMAP:
      glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_target);
      glTextureStorage2D(m_target, level, internalFormat, width, height);
      break;
    case TextureType::TEXTURE2D_ARRAY:
      glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_target);
      glTextureStorage3D(m_target, level, internalFormat, width, height, depth);
      break;
    case TextureType::CUBEMAP_ARRAY:
      glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &m_target);
      glTextureStorage3D(m_target, level, internalFormat, width, height, depth);
      break;
  }

  glTextureParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(m_target, GL_TEXTURE_BASE_LEVEL, 0);
  glTextureParameteri(m_target, GL_TEXTURE_MAX_LEVEL, level - 1);

  glTextureParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glGenerateTextureMipmap(m_target);
}

void
OpenGLTexture::UnBind() const noexcept {
  switch (m_imageDesc.textureType) {
    case TextureType::TEXTURE2D:
      glBindTexture(GL_TEXTURE_2D, 0);
      break;
    case TextureType::CUBEMAP:
      glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
      break;
    case TextureType::TEXTURE2D_ARRAY:
      glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
      break;
    case TextureType::CUBEMAP_ARRAY:
      glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);
      break;
  }
}

void
OpenGLTexture::SetData(void* data, size_t size, uint32_t level, uint32_t layer) {
  size_t bpp = 0;
  switch (m_imageDesc.format) {
    case TextureFormat::RED:
      bpp = 1;
      break;
    case TextureFormat::RG:
      bpp = 2;
      break;
    case TextureFormat::BGR:
    case TextureFormat::RGB:
      bpp = 3;
      break;
    case TextureFormat::RGBA:
    case TextureFormat::R32:
    case TextureFormat::BGRA:
    case TextureFormat::DEPTH:
      bpp = 4;
      break;
    case TextureFormat::RGB16F:
      bpp = 6;
      break;
    case TextureFormat::RGB32F:
      bpp = 12;
      break;
  }
  LOG_IF(ERROR, size != m_imageDesc.width * m_imageDesc.height * bpp)
      << "size and texture size do not match";

  auto dataFormat = ConvertToOpenglDataFormat(m_imageDesc.format);
  auto type = ConvertToOpenGLTextureType(m_imageDesc.format);
  auto width = m_imageDesc.width;
  auto height = m_imageDesc.height;

  switch (m_imageDesc.textureType) {
    case TextureType::TEXTURE2D:
      glTextureSubImage2D(m_target, level, 0, 0, width, height, dataFormat, type, data);
      break;
    case TextureType::CUBEMAP:
    case TextureType::TEXTURE2D_ARRAY:
      glTextureSubImage3D(m_target, level, 0, 0, layer, width, height, 1, dataFormat, type, data);
      break;
    case TextureType::CUBEMAP_ARRAY:
      uint32_t face = layer % 6;
      uint32_t _layer = layer / 6;
      glTextureSubImage3D(m_target, level, 0, 0, face, width, height, _layer, dataFormat, type,
                          data);
      break;
  }
}

GLenum
OpenGLTexture::GetOpenGLType() const {
  return ConvertToOpenGLTextureType(m_imageDesc.format);
}

GLenum
OpenGLTexture::GetOpenGLFormat() const {
  return ConvertToOpenglInternalFormat(m_imageDesc.format);
}

}  // namespace Marbas
