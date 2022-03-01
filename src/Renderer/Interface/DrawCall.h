#ifndef MARBARS_RENDER_INTERFACE_DRAWCALL_H
#define MARBARS_RENDER_INTERFACE_DRAWCALL_H

#include "Renderer/Interface/Shader.h"
#include "Renderer/Interface/VertexBuffer.h"
#include "Renderer/Interface/VertexArray.h"
#include "Renderer/Interface/IndexBuffer.h"

namespace Marbas {

class DrawCall {
public:
    DrawCall() = default;
    virtual ~DrawCall() = default;

public:

    virtual void AddVertices(const VertexBuffer* vertexBuffer,
                             const VertexArray* verticesArray) = 0;

    virtual void AddIndeices(const IndexBuffer* indices) = 0;

    virtual void AddShader(const Shader* shader) = 0;

    virtual void Link() = 0;

    virtual void Draw() = 0;

    virtual void Use() = 0;
};

}  // namespace Marbas

#endif
