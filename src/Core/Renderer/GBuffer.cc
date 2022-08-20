#include "Core/Renderer/GBuffer.hpp"

#include <glog/logging.h>

namespace Marbas {

GBuffer::GBuffer(uint32_t width, uint32_t height) : m_height(height), m_width(width) {}

std::shared_ptr<Texture2D>
GBuffer::GetTexture(GBufferTexutreType type) const {
  if (m_textures.find(type) == m_textures.end()) {
    LOG(ERROR) << "can't find texture in gbuffer";
    return nullptr;
  }

  return m_textures.at(type);
}

void
GBuffer::AddTexture(GBufferTexutreType type, uint32_t level, RHIFactory* rhiFactory) {
  LOG_IF(WARNING, m_textures.find(type) != m_textures.end())
      << "the texture has add to gbuffer, this will override it";

  std::shared_ptr<Texture2D> texture = nullptr;
  switch (type) {
    case GBufferTexutreType::COLOR:
      texture = rhiFactory->CreateTexutre2D(m_width, m_height, level, TextureFormat::RGBA);
      break;
    case GBufferTexutreType::NORMALS:
    case GBufferTexutreType::POSITION:
      texture = rhiFactory->CreateTexutre2D(m_width, m_height, level, TextureFormat::RGB32F);
      break;
    case GBufferTexutreType::DEPTH:
      texture = rhiFactory->CreateTexutre2D(m_width, m_height, level, TextureFormat::DEPTH);
      break;
  }
  m_textures[type] = texture;
}

Vector<std::tuple<GBufferTexutreType, std::shared_ptr<Texture2D>>>
GBuffer::GetAllTextures() const {
  Vector<std::tuple<GBufferTexutreType, std::shared_ptr<Texture2D>>> result;

  for (auto iter = m_textures.cbegin(); iter != m_textures.cend(); iter++) {
    result.push_back({iter->first, iter->second});
  }

  return result;
}

}  // namespace Marbas
