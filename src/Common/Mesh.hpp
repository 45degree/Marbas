#pragma once

#include "Common/Common.hpp"
#include "Common/Volume.hpp"
#include "Tool/Uid.hpp"
#include "Vertex.hpp"

namespace Marbas {

struct Mesh {
  String m_name;
  Vector<Vertex> m_vertices;
  Vector<uint32_t> m_indices;
  std::optional<Uid> m_materialId = std::nullopt;
  std::unique_ptr<Volume> m_volume = nullptr;

  bool m_needLoad = true;
};

}  // namespace Marbas
