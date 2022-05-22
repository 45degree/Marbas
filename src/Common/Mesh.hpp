#pragma once

#include "Common/Common.hpp"
#include "Tool/Uid.hpp"
#include "Vertex.hpp"

namespace Marbas {

struct Mesh {
  String m_name;
  Vector<Vertex> m_vertices;
  Vector<uint32_t> m_indices;
  std::optional<Uid> m_materialId = std::nullopt;
};

}  // namespace Marbas
