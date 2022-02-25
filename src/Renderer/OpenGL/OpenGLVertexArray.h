#ifndef MARBARS_RENDERER_OPENGL_VERTEX_ARRAY_H
#define MARBARS_RENDERER_OPENGL_VERTEX_ARRAY_H

#include "Common.h"
#include "Renderer/Interface/VertexArray.h"

namespace Marbas {

class OpenGLVertexArray : public VertexArray {
public:
    OpenGLVertexArray();
    ~OpenGLVertexArray() override;

public:
    void Bind() override;
    void UnBind() override;

    void EnableVertexAttribArray(const VertexBuffer& vertexBuffer) override;
private:
    GLuint VAO;
};

}  // namespace Marbas

#endif
