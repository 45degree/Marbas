#include "RenderGraphResource.hpp"

#include <variant>

namespace Marbas::details {

RenderGraphTexture::RenderGraphTexture(std::string_view name, RHIFactory* rhiFactory, const ImageCreateInfo& createInfo)
    : RenderGraphResource(name, rhiFactory), m_imageCreateInfo(createInfo) {}

RenderGraphTexture::~RenderGraphTexture() {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  for (auto& [desc, imageView] : m_imageViews) {
    bufCtx->DestroyImageView(imageView);
    imageView = nullptr;
  }
  bufCtx->DestroyImage(m_image);
}

void
RenderGraphTexture::Create() {
  if (m_isCreate) return;
  auto bufTex = m_rhiFactory->GetBufferContext();
  m_image = bufTex->CreateImage(m_imageCreateInfo);
  m_isCreate = true;
}

ImageView*
RenderGraphTexture::GetImageView(uint32_t layerBase, uint32_t layerCount, uint32_t levelBase, uint32_t levelCount) {
  SubresourceDesc desc;
  desc.levelCount = levelCount;
  desc.levelBase = levelBase;
  desc.layerBase = layerBase;
  desc.layerCount = layerCount;

  if (m_imageViews.find(desc) == m_imageViews.end()) {
    ImageViewCreateInfo createInfo;
    createInfo.layerCount = layerCount;
    createInfo.levelCount = levelCount;
    createInfo.image = m_image;
    createInfo.baseLevel = levelBase;
    createInfo.baseArrayLayer = layerBase;

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
      } else if constexpr (std::is_same_v<T, Image3DDesc>) {
        createInfo.type = ImageViewType::TEXTURE3D;
      }
    }, m_imageCreateInfo.imageDesc);
    // clang-format on

    auto imageView = m_rhiFactory->GetBufferContext()->CreateImageView(createInfo);
    m_imageViews.insert({desc, imageView});
  }

  return m_imageViews.at(desc);
}

}  // namespace Marbas::details
