#pragma once

#include <memory>

#include "Common/Common.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

enum class GBufferTexutreType {
  COLOR,
  POSITION,
  NORMALS,
  DEPTH,
  SHADOW_MAP,
  SHADOW_MAP_CUBE,
};

struct GBufferType {
  GBufferTexutreType type = GBufferTexutreType::COLOR;
  uint32_t levels = 1;
  uint32_t layers = 1;
};

class GBuffer final {
 public:
  GBuffer(uint32_t width, uint32_t height);

 public:
  void
  AddTexture(GBufferType gBufferType, RHIFactory* rhiFactory);

  Vector<std::tuple<GBufferTexutreType, std::shared_ptr<Texture>>>
  GetAllTextures() const;

  std::shared_ptr<Texture>
  GetTexture(GBufferTexutreType type) const;

 private:
  std::unordered_map<GBufferTexutreType, std::shared_ptr<Texture>> m_textures;
  uint32_t m_width;
  uint32_t m_height;
};

}  // namespace Marbas
