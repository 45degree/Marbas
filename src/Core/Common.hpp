#pragma once

#include "AssetManager/Vertex.hpp"
#include "Common/Common.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

inline Vector<InputElementDesc>
GetMeshVertexInfoLayout() {
  Vector<InputElementDesc> layouts{
      {0, ElementType::R32G32B32_SFLOAT, 0, offsetof(Vertex, posX), 0},
      {0, ElementType::R32G32B32_SFLOAT, 1, offsetof(Vertex, normalX), 0},
      {0, ElementType::R32G32_SFLOAT, 2, offsetof(Vertex, textureU), 0},
      {0, ElementType::R32G32B32_SFLOAT, 3, offsetof(Vertex, tangentX), 0},
      {0, ElementType::R32G32B32_SFLOAT, 4, offsetof(Vertex, bitangentX), 0},
  };

  return layouts;
};

inline Vector<InputElementView>
GetMeshVertexViewInfo() {
  Vector<InputElementView> views = {
      {.binding = 0, .stride = sizeof(Vertex), .inputClass = VertexInputClass::VERTEX},
  };
  return views;
}

}  // namespace Marbas
