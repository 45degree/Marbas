#include "RHI/OpenGL/OpenGLTexture.hpp"

#include <glog/logging.h>

namespace Marbas {

GLenum
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
    case TextureFormat::RG16F:
    case TextureFormat::RG32F:
      return GL_RG;
    case TextureFormat::RGBA:
      return GL_RGBA;
    case TextureFormat::RGB:
    case TextureFormat::RGB16F:
    case TextureFormat::RGB32F:
      return GL_RGB;
    case TextureFormat::DEPTH:
      return GL_DEPTH_COMPONENT;
  }
}

GLenum
ConvertToOpenGLTextureType(TextureType type) {
  switch (type) {
    case TextureType::TEXTURE2D:
      return GL_TEXTURE_2D;
    case TextureType::TEXTURE2D_ARRAY:
      return GL_TEXTURE_2D_ARRAY;
    case TextureType::CUBEMAP:
      return GL_TEXTURE_CUBE_MAP;
    case TextureType::CUBEMAP_ARRAY:
      return GL_TEXTURE_CUBE_MAP_ARRAY;
    default:
      // never enter this line
      DLOG_ASSERT(false);
  }
}

GLenum
ConvertToOpenGLTextureDataType(TextureFormat type) {
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
    case TextureFormat::RG16F:
    case TextureFormat::RG32F:
    case TextureFormat::RGB16F:
    case TextureFormat::RGB32F:
      return GL_FLOAT;
    case TextureFormat::DEPTH:
      return GL_FLOAT;
  }
  return GL_UNSIGNED_BYTE;
}

GLenum
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
    case TextureFormat::RG16F:
      return GL_RG16F;
    case TextureFormat::RG32F:
      return GL_RG32F;
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

  if (level != 1) {
    glTextureParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  } else {
    glTextureParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }

  glTextureParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(m_target, GL_TEXTURE_BASE_LEVEL, 0);
  glTextureParameteri(m_target, GL_TEXTURE_MAX_LEVEL, level - 1);

  glTextureParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void
OpenGLTexture::UnBind() const noexcept {
  if (m_bindingPoint.has_value()) {
    glBindTextureUnit(*m_bindingPoint, 0);
  }
}

void
OpenGLTexture::SetData(void* data, size_t size, uint32_t level, uint32_t layer) {
  auto dataFormat = ConvertToOpenglDataFormat(m_imageDesc.format);
  auto type = ConvertToOpenGLTextureDataType(m_imageDesc.format);
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

  if (m_imageDesc.mipmapLevel > 1 && level < m_imageDesc.mipmapLevel) {
    glGenerateTextureMipmap(m_target);
  }
}

GLenum
OpenGLTexture::GetOpenGLType() const {
  return ConvertToOpenGLTextureDataType(m_imageDesc.format);
}

GLenum
OpenGLTexture::GetOpenGLFormat() const {
  return ConvertToOpenglInternalFormat(m_imageDesc.format);
}

}  // namespace Marbas
