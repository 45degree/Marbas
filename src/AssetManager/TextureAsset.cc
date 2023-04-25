#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "TextureAsset.hpp"

#include <glog/logging.h>
#include <stb_image.h>

#include "AssetException.hpp"

namespace Marbas {

std::shared_ptr<TextureAsset>
TextureAsset::Load(const Path& path, bool flipV) {
  auto filename = path.string();

#ifdef _WIN32
  std::replace(filename.begin(), filename.end(), '/', '\\');
#elif __linux__
  std::replace(filename.begin(), filename.end(), '\\', '/');
#endif

  // load image
  void* data;
  int width, height;
  ImageFormat format;
  if (flipV) {
    stbi_set_flip_vertically_on_load(true);
  }
  if (stbi_is_hdr(filename.c_str())) {
    data = stbi_loadf(filename.c_str(), &width, &height, nullptr, STBI_rgb_alpha);
    format = ImageFormat::RGBA32F;
  } else {
    data = stbi_load(filename.c_str(), &width, &height, nullptr, STBI_rgb_alpha);
    format = ImageFormat::RGBA;
  }
  stbi_set_flip_vertically_on_load(false);

  if (data == nullptr) {
    LOG(WARNING) << FORMAT("can't load texture: {}", filename);
    throw AssetException("can't load texture", filename);
  }

  // copy image data
  auto asset = std::make_shared<TextureAsset>();
  asset->m_data.resize(width * height * sizeof(unsigned char) * 4);
  std::memcpy(asset->m_data.data(), data, asset->m_data.size());
  asset->m_width = width;
  asset->m_height = height;
  asset->m_format = format;

  stbi_image_free(data);
  return asset;
}

}  // namespace Marbas
