#pragma once

#include <optional>

#include "Common/Common.hpp"
#include "RHI/Interface/IDescriptor.hpp"

namespace Marbas {

enum class TextureFormat {
  RED,
  RG,
  RGB,
  BGR,
  RGBA,
  BGRA,
  RGB16F,
  RGB32F,
  DEPTH,
};

enum TextureType {
  TEXTURE2D,
  CUBEMAP,
};

class Texture {
 public:
  explicit Texture(TextureType type, TextureFormat format) : m_type(type), m_format(format) {}
  virtual ~Texture() = default;

 public:
  TextureType
  GetTextureType() const noexcept {
    return m_type;
  }

  TextureFormat
  GetFormat() const noexcept {
    return m_format;
  }

 protected:
  TextureType m_type;
  TextureFormat m_format;
};

// TODO: refactor the texture create info

class Texture2D : public Texture {
 public:
  Texture2D(uint32_t width, uint32_t height, uint32_t level, TextureFormat formatType)
      : Texture(TextureType::TEXTURE2D, formatType),
        m_width(width),
        m_height(height),
        m_level(level) {}

  virtual ~Texture2D() = default;

  virtual void
  SetData(void* data, uint32_t size) = 0;

  virtual void*
  GetTexture() = 0;

  uint32_t
  GetWidth() const noexcept {
    return m_width;
  }

  uint32_t
  GetHeight() const noexcept {
    return m_height;
  }

  uint32_t
  GetDepth() const noexcept {
    switch (m_format) {
      case TextureFormat::DEPTH:
      case TextureFormat::RED:
        return 1;
      case TextureFormat::RG:
        return 2;
      case TextureFormat::BGR:
      case TextureFormat::RGB:
      case TextureFormat::RGB16F:
      case TextureFormat::RGB32F:
        return 3;
      case TextureFormat::BGRA:
      case TextureFormat::RGBA:
        return 4;
    }
    return 3;
  }

  uint32_t
  GetLevel() const noexcept {
    return m_level;
  }

 protected:
  uint32_t m_width;
  uint32_t m_height;
  uint32_t m_level;
};

/**
 * @brief
 */

enum class CubeMapPosition {
  BACK,
  BOTTOM,
  FRONT,
  LEFT,
  RIGHT,
  TOP,
};

class TextureCubeMap : public Texture {
 public:
  TextureCubeMap(int width, int height, TextureFormat formatType)
      : Texture(TextureType::CUBEMAP, formatType), m_width(width), m_height(height) {}

  virtual ~TextureCubeMap() = default;

 public:
  virtual void
  SetData(void* data, uint32_t size, CubeMapPosition position) = 0;

 protected:
  int m_width;
  int m_height;
};

}  // namespace Marbas
