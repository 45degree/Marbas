#pragma once

#include <array>
#include <cereal/types/array.hpp>
#include <cereal/types/optional.hpp>

#include "AssetManager/AssetPath.hpp"
#include "Common/Common.hpp"

namespace Marbas {

struct Material {
  std::optional<AssetPath> m_diffuseTexturePath;
  std::array<float, 4> m_diffuseColor;
  bool m_useDiffuseTexture = false;

  std::optional<AssetPath> m_normalTexturePath;
  bool m_useNormalTexture = false;

  std::optional<AssetPath> m_roughnessTexturePath;
  float m_roughnessValue = 0;
  bool m_useRoughnessTexture = false;

  std::optional<AssetPath> m_metalnessTexturePath;
  float m_metalnessValue = 0;
  bool m_useMetalnessTexture = false;

  template <typename Archive>
  void
  serialize(Archive& ar) {
    ar(m_diffuseTexturePath, m_diffuseColor, m_useDiffuseTexture,        //
       m_normalTexturePath, m_useNormalTexture,                          //
       m_roughnessTexturePath, m_roughnessValue, m_useRoughnessTexture,  //
       m_metalnessTexturePath, m_metalnessValue, m_useMetalnessTexture   //
    );
  }
};

}  // namespace Marbas
