#include "Core/Renderer/GBuffer.hpp"

#include <glog/logging.h>

namespace Marbas {

struct _GBufferTextureInfo {
  TextureFormat format;
  TextureType type;
};

using GBufferTextureLookUpTableType = std::unordered_map<GBufferTexutreType, _GBufferTextureInfo>;
const static GBufferTextureLookUpTableType gBufferTextureLookUpTable = {
    {GBufferTexutreType::COLOR, {TextureFormat::RGBA, TextureType::TEXTURE2D}},
    {GBufferTexutreType::POSITION, {TextureFormat::RGB32F, TextureType::TEXTURE2D}},
    {GBufferTexutreType::NORMALS, {TextureFormat::RGB32F, TextureType::TEXTURE2D}},
    {GBufferTexutreType::DEPTH, {TextureFormat::DEPTH, TextureType::TEXTURE2D}},
    {GBufferTexutreType::AMBIENT_OCCLUSION, {TextureFormat::R32, TextureType::TEXTURE2D}},
    {GBufferTexutreType::ROUGHTNESS, {TextureFormat::R32, TextureType::TEXTURE2D}},
    {GBufferTexutreType::METALLIC, {TextureFormat::R32, TextureType::TEXTURE2D}},
    {GBufferTexutreType::HDR_IMAGE, {TextureFormat::RGB16F, TextureType::CUBEMAP}},
    {GBufferTexutreType::PRE_FILTER_CUBEMAP, {TextureFormat::RGB16F, TextureType::CUBEMAP}},
    {GBufferTexutreType::IBL_BRDF_LOD, {TextureFormat::RG16F, TextureType::TEXTURE2D}},
    {GBufferTexutreType::SHADOW_MAP, {TextureFormat::DEPTH, TextureType::TEXTURE2D_ARRAY}},
    {GBufferTexutreType::SHADOW_MAP_CUBE, {TextureFormat::DEPTH, TextureType::CUBEMAP_ARRAY}},
};

GBuffer::GBuffer(uint32_t width, uint32_t height, RHIFactory* rhiFactory, GBufferTexutreType type,
                 uint32_t levels, uint32_t layers)
    : m_height(height), m_width(width), m_type(type), m_levels(levels), m_layers(layers) {
  auto textureInfo = gBufferTextureLookUpTable.at(m_type);
  DLOG_ASSERT(textureInfo.type != TextureType::TEXTURE2D || layers == 1);

  ImageDesc desc{
      .textureType = textureInfo.type,
      .format = textureInfo.format,
      .width = m_width,
      .height = m_height,
      .arrayLayer = m_layers,
      .mipmapLevel = m_levels,
  };
  m_texture = rhiFactory->CreateTexture(desc);
}

std::shared_ptr<Texture>
GBuffer::GetTexture() const {
  return m_texture;
}

}  // namespace Marbas
