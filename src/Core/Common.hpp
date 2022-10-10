#pragma once

#include "RHI/Interface/VertexBuffer.hpp"

namespace Marbas {

inline Vector<ElementLayout>
GetMeshVertexInfoLayout() {
  Vector<ElementLayout> layouts{
      ElementLayout{0, ElementType::FLOAT, sizeof(float), 3, false, 0, 0},
      ElementLayout{1, ElementType::FLOAT, sizeof(float), 3, false, 0, 0},
      ElementLayout{2, ElementType::FLOAT, sizeof(float), 2, false, 0, 0},
      ElementLayout{3, ElementType::FLOAT, sizeof(float), 3, false, 0, 0},
      ElementLayout{4, ElementType::FLOAT, sizeof(float), 3, false, 0, 0},
  };

  ElementLayout::CalculateLayout(layouts);

  return layouts;
};

}  // namespace Marbas
