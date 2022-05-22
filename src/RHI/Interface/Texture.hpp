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
  DEPTH,
};

class Texture2D {
 public:
  Texture2D(uint32_t width, uint32_t height, TextureFormat formatType)
      : m_width(width), m_height(height), m_format(formatType) {}

  virtual ~Texture2D() = default;

  virtual void
  SetData(void* data, uint32_t size) = 0;

  virtual void*
  GetTexture() = 0;

  virtual std::shared_ptr<IImageDescriptor>
  GetDescriptor() const = 0;

  uint32_t
  GetWidth() const noexcept {
    return m_width;
  }

  uint32_t
  GetHeight() const noexcept {
    return m_height;
  }

  TextureFormat
  GetFormat() const noexcept {
    return m_format;
  }

 protected:
  uint32_t m_width;
  uint32_t m_height;
  TextureFormat m_format;
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

class TextureCubeMap {
 public:
  TextureCubeMap(int width, int height, TextureFormat formatType)
      : m_width(width), m_height(height), m_format(formatType) {}

  virtual ~TextureCubeMap() = default;

 public:
  virtual void
  SetData(void* data, uint32_t size, CubeMapPosition position) = 0;
  virtual std::shared_ptr<IImageDescriptor>
  GetDescriptor() const = 0;

 protected:
  int m_width;
  int m_height;
  TextureFormat m_format;
};

}  // namespace Marbas
