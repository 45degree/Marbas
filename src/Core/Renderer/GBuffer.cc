#include "Core/Renderer/GBuffer.hpp"

#include <glog/logging.h>

namespace Marbas {

GBuffer::GBuffer(uint32_t width, uint32_t height, uint32_t usage, RHIFactory* rhiFactory,
                 const GBufferMetaInfo& metaInfo)
    : m_height(height), m_width(width), m_bufferContext(rhiFactory->GetBufferContext()) {
  Recreate(width, height, usage, metaInfo);
}

GBuffer::~GBuffer() {
  m_bufferContext->DestroyImage(m_texture);
  m_bufferContext->DestroyImageView(m_wholeImageView);
}

void
GBuffer::Recreate(uint32_t width, uint32_t height, uint32_t usage, const GBufferMetaInfo& metaInfo) {
  m_width = width;
  m_height = height;
  if (m_texture != nullptr) {
    m_bufferContext->DestroyImage(m_texture);
  }
  if (m_wholeImageView != nullptr) {
    m_bufferContext->DestroyImageView(m_wholeImageView);
  }

  /**
   * create image
   */
  ImageCreateInfo imageCreateInfo;
  imageCreateInfo.format = metaInfo.format;
  imageCreateInfo.height = m_height;
  imageCreateInfo.width = m_width;
  imageCreateInfo.mipMapLevel = metaInfo.levels;
  imageCreateInfo.usage = usage;
  imageCreateInfo.sampleCount = SampleCount::BIT1;

  switch (metaInfo.imageViewType) {
    case ImageViewType::TEXTURE2D: {
      imageCreateInfo.imageDesc = Image2DDesc();
      m_imageViewType = ImageViewType::TEXTURE2D;
      break;
    }
    case ImageViewType::CUBEMAP: {
      imageCreateInfo.imageDesc = CubeMapImageDesc();
      m_imageViewType = ImageViewType::CUBEMAP;
      break;
    }
    case ImageViewType::TEXTURE2D_ARRAY: {
      Image2DArrayDesc image2DArrayDesc;
      m_imageViewType = ImageViewType::TEXTURE2D_ARRAY;
      image2DArrayDesc.arraySize = metaInfo.layers;
      imageCreateInfo.imageDesc = image2DArrayDesc;
      break;
    }
    case ImageViewType::CUBEMAP_ARRAY: {
      m_imageViewType = ImageViewType::CUBEMAP_ARRAY;
      CubeMapArrayImageDesc cubeMapArrayImageDesc;
      cubeMapArrayImageDesc.arraySize = metaInfo.layers;
      imageCreateInfo.imageDesc = cubeMapArrayImageDesc;
      break;
    }
  }
  m_texture = m_bufferContext->CreateImage(imageCreateInfo);

  ImageViewCreateInfo imageViewCreateInfo;
  imageViewCreateInfo.baseArrayLayer = 0;
  imageViewCreateInfo.layerCount = metaInfo.layers;
  imageViewCreateInfo.baseLevel = 0;
  imageViewCreateInfo.levelCount = metaInfo.levels;
  imageViewCreateInfo.type = m_imageViewType;
  imageViewCreateInfo.image = m_texture;

  m_wholeImageView = m_bufferContext->CreateImageView(imageViewCreateInfo);
}

GBufferRegistry::GBufferRegistry() {
  Emplace<GBuffer_Color>();
  Emplace<GBuffer_Position>();
  Emplace<GBuffer_Normals>();
  Emplace<GBuffer_Depth>();
  Emplace<GBuffer_AmbientOcclusion>();
  Emplace<GBuffer_Roughness>();
  Emplace<GBuffer_Metallic>();
  Emplace<GBuffer_HDR>();
  Emplace<GBuffer_PreFilterCubemap>();
  Emplace<GBuffer_IBL_BRDF_LOD>();
  Emplace<GBuffer_ShadowMap>();
  Emplace<GBuffer_ShadowMapCube>();
}

}  // namespace Marbas
