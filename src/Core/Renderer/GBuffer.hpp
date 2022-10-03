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
  AMBIENT_OCCLUSION,
  ROUGHTNESS,
  METALLIC,
  HDR_IMAGE,
  PRE_FILTER_CUBEMAP,
  IBL_BRDF_LOD,
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
  GBuffer(uint32_t width, uint32_t height, RHIFactory* rhiFactory,
          GBufferTexutreType type = GBufferTexutreType::COLOR, uint32_t levels = 1,
          uint32_t layers = 1);

 public:
  // void
  // AddTexture(GBufferType gBufferType, RHIFactory* rhiFactory);

  // Vector<std::tuple<GBufferTexutreType, std::shared_ptr<Texture>>>
  // GetAllTextures() const;

  std::shared_ptr<Texture>
  GetTexture() const;

 private:
  // std::unordered_map<GBufferTexutreType, std::shared_ptr<Texture>> m_textures;

  GBufferTexutreType m_type;
  uint32_t m_levels;
  uint32_t m_layers;
  std::shared_ptr<Texture> m_texture;

  uint32_t m_width;
  uint32_t m_height;
};

}  // namespace Marbas
