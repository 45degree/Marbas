#include "TextureGPUData.hpp"

namespace Marbas {

Task<>
TextureGPUData::Load(const Asset& asset) {
  auto bufCtx = m_rhiFactory->GetBufferContext();

  m_imageCreateInfo.usage = ImageUsageFlags::SHADER_READ;
  m_imageCreateInfo.width = asset.m_width;
  m_imageCreateInfo.height = asset.m_height;
  m_imageCreateInfo.format = asset.m_format;
  m_imageCreateInfo.mipMapLevel = 1;
  m_imageCreateInfo.sampleCount = SampleCount::BIT1;
  m_imageCreateInfo.imageDesc = Image2DDesc();

  m_image = bufCtx->CreateImage(m_imageCreateInfo);

  bufCtx->UpdateImage(UpdateImageInfo{
      .image = m_image,
      .level = 0,
      .xOffset = 0,
      .yOffset = 0,
      .zOffset = 0,
      .width = static_cast<int32_t>(asset.m_width),
      .height = static_cast<int32_t>(asset.m_height),
      .depth = 1,
      .data = const_cast<void*>(static_cast<const void*>(asset.m_data.data())),
      .dataSize = static_cast<uint32_t>(asset.m_data.size()),
  });

  co_return;
}

Task<>
TextureGPUData::Update(const Asset& asset) {
  // TODO: Implement
  co_return;
}

ImageView*
TextureGPUData::GetImageView(uint32_t baseLayer, uint32_t layerCount, uint32_t baseLevel, uint32_t levelCount) {
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

ImTextureID
TextureGPUData::GetImGuiTextureId(uint32_t baseLayer, uint32_t layerCount, uint32_t baseLevel, uint32_t levelCount) {
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

TextureGPUData::~TextureGPUData() {
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

}  // namespace Marbas
