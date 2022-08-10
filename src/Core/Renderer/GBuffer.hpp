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
};

class GBuffer final {
 public:
  GBuffer(uint32_t width, uint32_t height);

 public:
  void
  AddTexture(GBufferTexutreType type, uint32_t level, RHIFactory* rhiFactory);

  Vector<std::tuple<GBufferTexutreType, std::shared_ptr<Texture2D>>>
  GetAllTextures() const;

  std::shared_ptr<Texture2D>
  GetTexture(GBufferTexutreType type) const;

 private:
  std::unordered_map<GBufferTexutreType, std::shared_ptr<Texture2D>> m_textures;
  uint32_t m_width;
  uint32_t m_height;
};

}  // namespace Marbas
