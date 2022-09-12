#include "Core/Renderer/GBuffer.hpp"

#include <glog/logging.h>

namespace Marbas {

GBuffer::GBuffer(uint32_t width, uint32_t height) : m_height(height), m_width(width) {}

std::shared_ptr<Texture>
GBuffer::GetTexture(GBufferTexutreType type) const {
  if (m_textures.find(type) == m_textures.end()) {
    LOG(ERROR) << "can't find texture in gbuffer";
    return nullptr;
  }

  return m_textures.at(type);
}

void
GBuffer::AddTexture(GBufferType gBufferType, RHIFactory* rhiFactory) {
  LOG_IF(WARNING, m_textures.find(gBufferType.type) != m_textures.end())
      << "the texture has add to gbuffer, this will override it";

  std::shared_ptr<Texture> texture = nullptr;
  ImageDesc desc{
      .width = m_width,
      .height = m_height,
      .mipmapLevel = gBufferType.levels,
  };

  switch (gBufferType.type) {
    case GBufferTexutreType::COLOR:
      desc.format = TextureFormat::RGBA;
      desc.textureType = TextureType::TEXTURE2D;
      desc.arrayLayer = 1;
      texture = rhiFactory->CreateTexture(desc);
      break;
    case GBufferTexutreType::NORMALS:
    case GBufferTexutreType::POSITION:
      desc.format = TextureFormat::RGB32F;
      desc.textureType = TextureType::TEXTURE2D;
      desc.arrayLayer = 1;
      texture = rhiFactory->CreateTexture(desc);
      break;
    case GBufferTexutreType::DEPTH:
      desc.format = TextureFormat::DEPTH;
      desc.textureType = TextureType::TEXTURE2D;
      desc.arrayLayer = 1;
      texture = rhiFactory->CreateTexture(desc);
      break;
    case GBufferTexutreType::SHADOW_MAP:
      desc.format = TextureFormat::DEPTH;
      desc.textureType = TextureType::TEXTURE2D_ARRAY;
      desc.arrayLayer = gBufferType.layers;
      texture = rhiFactory->CreateTexture(desc);
      break;
    case GBufferTexutreType::SHADOW_MAP_CUBE:
      desc.format = TextureFormat::DEPTH;
      desc.textureType = TextureType::CUBEMAP_ARRAY;
      desc.arrayLayer = gBufferType.layers * 6;
      texture = rhiFactory->CreateTexture(desc);
      break;
  }
  m_textures[gBufferType.type] = texture;
}

Vector<std::tuple<GBufferTexutreType, std::shared_ptr<Texture>>>
GBuffer::GetAllTextures() const {
  Vector<std::tuple<GBufferTexutreType, std::shared_ptr<Texture>>> result;

  for (auto iter = m_textures.cbegin(); iter != m_textures.cend(); iter++) {
    result.push_back({iter->first, iter->second});
  }

  return result;
}

}  // namespace Marbas
