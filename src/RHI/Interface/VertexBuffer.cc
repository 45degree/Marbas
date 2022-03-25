#include "RHI/Interface/VertexBuffer.h"

#include <algorithm>

namespace Marbas {

void VertexBuffer::SetLayout(const Vector<ElementLayout>& layout) {
    m_layout = layout;

    // calculate the stride and offset for each element
 
    std::size_t stride = 0;
    for(const auto& elementInfo : m_layout) {
        stride += sizeof(float) * elementInfo.size;
    }

    std::size_t offset = 0;
    for(int i = 0; i < this->m_layout.size(); i++) {
        m_layout[i].offset = offset;
        m_layout[i].stride = stride;
        offset += sizeof(float) * m_layout[i].size;
    }
}

}  // namespace Marbas
