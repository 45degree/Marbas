#pragma once

#include <cereal/types/vector.hpp>
#include <vector>

#include "AssetManager/AssetManager.hpp"
#include "Common/Common.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

struct TextureAsset final : public AssetBase {
  std::vector<unsigned char> m_data;
  uint32_t m_width;
  uint32_t m_height;
  ImageFormat m_format;

  template <typename Archive>
  void
  serialize(Archive& ar) {
    ar(m_uid, m_width, m_height, m_data, m_format);
  }

  static std::shared_ptr<TextureAsset>
  Load(const Path& m_path, bool flipV = false);
};

}  // namespace Marbas
