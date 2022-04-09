#ifndef MARBARS_RHI_INTERFACE_INDEX_BUFFER_H
#define MARBARS_RHI_INTERFACE_INDEX_BUFFER_H

#include "Common.hpp"

namespace Marbas {

class IndexBuffer {
public:
    IndexBuffer() = default;
    virtual ~IndexBuffer() = default;
public:
    virtual void Bind() const = 0;
    virtual void UnBind() const = 0;
    virtual void SetData(const Vector<uint32_t>& data) = 0;

    [[nodiscard]] size_t GetIndexCount() const noexcept { return indexCount; }

protected:
    size_t indexCount = 0;
};

}  // namespace Marbas

#endif
