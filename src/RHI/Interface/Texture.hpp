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
  R32,
  RGB16F,
  RGB32F,
  DEPTH,
};

enum TextureType {
  TEXTURE2D,
  CUBEMAP,
  TEXTURE2D_ARRAY,
  CUBEMAP_ARRAY,
};

class Texture;
struct ImageView {
  virtual void
  SetTexture(const std::shared_ptr<Texture>& texture) = 0;

  virtual void
  SetRangeInfo(uint32_t layerBase, uint32_t layerCount, uint32_t levelBase,
               uint32_t levelCount) = 0;
};

struct ImageViewDesc final {
  uint32_t m_layerBase = 0;
  uint32_t m_layerCount = 1;
  uint32_t m_levelBase = 0;
  uint32_t m_levelCount = 1;
};

struct ImageDesc final {
  TextureType textureType = TextureType::TEXTURE2D;
  TextureFormat format = TextureFormat::RGBA;
  uint32_t width = 1920;
  uint32_t height = 1080;
  uint32_t arrayLayer = 1;
  uint32_t mipmapLevel = 1;
};

class Texture {
 public:
  Texture(const ImageDesc& imageDesc) : m_imageDesc(imageDesc) {}

 public:
  virtual void
  SetData(void* data, size_t size, uint32_t level = 0, uint32_t layer = 0) = 0;

  /**
   * @brief get origin handle, in OpenGl it means GLUint,
   *        in Vulkan, it's a pointer to the vk::Image
   */
  virtual void*
  GetOriginHandle() = 0;

  uint32_t
  GetLevels() const {
    return m_imageDesc.mipmapLevel;
  }

  uint32_t
  GetLayers() const {
    return m_imageDesc.arrayLayer;
  }

  uint32_t
  GetWidth() const {
    return m_imageDesc.width;
  }

  uint32_t
  GetHeight() const {
    return m_imageDesc.height;
  }

  TextureFormat
  GetFormat() const {
    return m_imageDesc.format;
  }

  TextureType
  GetTextureType() const {
    return m_imageDesc.textureType;
  }

 protected:
  const ImageDesc m_imageDesc;
};

}  // namespace Marbas
