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

TextureGPUAsset::~TextureGPUAsset() {
  auto* bufCtx = m_rhiFactory->GetBufferContext();
  auto* imguiCtx = m_rhiFactory->GetImguiContext();

  for (auto&& [subResDesc, id] : m_imguiTextures) {
    imguiCtx->DestroyImGuiImage(id);
  }

  for (auto&& [subResDesc, imageView] : m_imageViews) {
    bufCtx->DestroyImageView(imageView);
  }

  if (m_image != nullptr) {
    bufCtx->DestroyImage(m_image);
  }
}

ImageView*
TextureGPUAsset::GetImageView(uint32_t baseLayer, uint32_t layerCount, uint32_t baseLevel, uint32_t levelCount) {
  SubresourceDesc desc;
  desc.levelCount = levelCount;
  desc.levelBase = baseLevel;
  desc.layerBase = baseLayer;
  desc.layerCount = layerCount;

  if (m_imageViews.find(desc) != m_imageViews.end()) {
    return m_imageViews.at(desc);
  }

  ImageViewCreateInfo createInfo;
  createInfo.layerCount = layerCount;
  createInfo.levelCount = levelCount;
  createInfo.image = m_image;
  createInfo.baseLevel = baseLevel;
  createInfo.baseArrayLayer = baseLayer;

  // clang-format off
  std::visit([&](auto&& imageDesc) {
    using T = std::decay_t<decltype(imageDesc)>;
    if constexpr (std::is_same_v<T, Image2DDesc>) {
      createInfo.type = ImageViewType::TEXTURE2D;
    } else if constexpr (std::is_same_v<T, Image2DArrayDesc>) {
      createInfo.type = ImageViewType::TEXTURE2D_ARRAY;
    } else if constexpr (std::is_same_v<T, CubeMapImageDesc>) {
      createInfo.type = ImageViewType::CUBEMAP;
    } else if constexpr (std::is_same_v<T, CubeMapArrayImageDesc>) {
      createInfo.type = ImageViewType::CUBEMAP_ARRAY;
    }
  }, m_imageCreateInfo.imageDesc);
  // clang-format on

  auto imageView = m_rhiFactory->GetBufferContext()->CreateImageView(createInfo);
  m_imageViews.insert({desc, imageView});

  return m_imageViews.at(desc);
}

std::shared_ptr<TextureGPUAsset>
TextureGPUAsset::LoadToGPU(const std::shared_ptr<TextureAsset>& asset, RHIFactory* rhiFactory) {
  auto gpuAsset = std::make_shared<TextureGPUAsset>(rhiFactory);

  auto bufCtx = rhiFactory->GetBufferContext();

  gpuAsset->m_imageCreateInfo.usage = ImageUsageFlags::SHADER_READ;
  gpuAsset->m_imageCreateInfo.width = asset->m_width;
  gpuAsset->m_imageCreateInfo.height = asset->m_height;
  gpuAsset->m_imageCreateInfo.format = asset->m_format;
  gpuAsset->m_imageCreateInfo.mipMapLevel = 1;
  gpuAsset->m_imageCreateInfo.sampleCount = SampleCount::BIT1;
  gpuAsset->m_imageCreateInfo.imageDesc = Image2DDesc();

  gpuAsset->m_image = bufCtx->CreateImage(gpuAsset->m_imageCreateInfo);

  bufCtx->UpdateImage(UpdateImageInfo{
      .image = gpuAsset->m_image,
      .level = 0,
      .xOffset = 0,
      .yOffset = 0,
      .zOffset = 0,
      .width = static_cast<int32_t>(asset->m_width),
      .height = static_cast<int32_t>(asset->m_height),
      .depth = 1,
      .data = asset->m_data.data(),
      .dataSize = static_cast<uint32_t>(asset->m_data.size()),
  });

  return gpuAsset;
}

ImTextureID
TextureGPUAsset::GetImGuiTextureId(uint32_t baseLayer, uint32_t layerCount, uint32_t baseLevel, uint32_t levelCount) {
  SubresourceDesc desc;
  desc.levelCount = levelCount;
  desc.levelBase = baseLevel;
  desc.layerBase = baseLayer;
  desc.layerCount = layerCount;

  if (m_imguiTextures.find(desc) != m_imguiTextures.end()) {
    return m_imguiTextures.at(desc);
  }

  auto imguiContext = m_rhiFactory->GetImguiContext();
  auto imageView = GetImageView(baseLayer, layerCount, baseLevel, levelCount);
  auto id = imguiContext->CreateImGuiImage(imageView);
  m_imguiTextures[desc] = id;
  return id;
}

}  // namespace Marbas
