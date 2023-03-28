#pragma once

#include <optional>

#include "Common/Common.hpp"
#include "Material.hpp"
#include "Uid.hpp"
#include "Vertex.hpp"

namespace Marbas {

struct Mesh {
  std::string m_name;
  Vector<Vertex> m_vertices;
  Vector<uint32_t> m_indices;
  Material m_material;

  template <typename Archive>
  void
  serialize(Archive& ar) {
    ar(m_name, m_vertices, m_indices, m_material);
  }
};

}  // namespace Marbas
