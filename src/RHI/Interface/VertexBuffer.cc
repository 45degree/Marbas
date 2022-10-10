#include "RHI/Interface/VertexBuffer.hpp"

#include <algorithm>

namespace Marbas {

static int
GetTypeSize(ElementType type) {
  switch (type) {
    case ElementType::BYTE:
    case ElementType::UNSIGNED_BYTE:
      return 1;
    case ElementType::SHORT:
    case ElementType::UNSIGNED_SHORT:
    case ElementType::HALF_FLOAT:
      return 2;
    case ElementType::INT:
    case ElementType::UNSIGNED_INT:
    case ElementType::FLOAT:
      return 4;
    case ElementType::DOUBLE:
      return 8;
  }
  return 0;
}

void
ElementLayout::CalculateLayout(Vector<ElementLayout>& layouts) {
  size_t stride = 0;
  for (auto& layout : layouts) {
    layout.typeBytes = GetTypeSize(layout.mateType);
    stride += layout.typeBytes * layout.count;
  }

  std::size_t offset = 0;
  for (int i = 0; i < layouts.size(); i++) {
    layouts[i].offset = offset;
    layouts[i].stride = stride;
    offset += layouts[i].typeBytes * layouts[i].count;
  }
}

void
VertexBuffer::SetLayout(const Vector<ElementLayout>& layout) {
  m_layout = layout;
  m_stride = layout[0].stride;

  // calculate the stride and offset for each element
  // size_t stride = 0;
  // for (auto& layout : m_layout) {
  //   layout.typeBytes = GetTypeSize(layout.mateType);
  //   stride += layout.typeBytes * layout.count;
  // }
  // m_stride = stride;
  //
  // std::size_t offset = 0;
  // for (int i = 0; i < this->m_layout.size(); i++) {
  //   m_layout[i].offset = offset;
  //   m_layout[i].stride = stride;
  //   offset += m_layout[i].typeBytes * m_layout[i].count;
  // }
}

}  // namespace Marbas
