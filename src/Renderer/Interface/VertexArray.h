#ifndef MARBARS_RENDERER_INTERFACE_VERTEX_ARRAY_H
#define MARBARS_RENDERER_INTERFACE_VERTEX_ARRAY_H

#include "Common.h"
#include "Renderer/Interface/VertexBuffer.h"

namespace Marbas {

class VertexArray {
public:
    VertexArray() = default;
    virtual ~VertexArray() = default;

public:
    virtual void Bind() = 0;
    virtual void UnBind() = 0;

    virtual void EnableVertexAttribArray(const VertexBuffer& vertexBuffer) = 0;
};

}  // namespace Marbas

#endif
