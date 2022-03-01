#ifndef MARBARS_RENDERER_OPENGL_DRAWCALL_H
#define MARBARS_RENDERER_OPENGL_DRAWCALL_H

#include "Renderer/Interface/DrawCall.h"
#include "Renderer/Interface/IndexBuffer.h"

namespace Marbas {

class OpenGLDrawCall : public DrawCall{
public:
    OpenGLDrawCall();
    ~OpenGLDrawCall() override;

public:

    void AddVertices(const VertexBuffer* vertexBuffer, const VertexArray* verticesArray) override;

    void AddIndeices(const IndexBuffer* indices) override;

    void AddShader(const Shader* shader) override;

    void Link() override;

    void Draw() override;

    void Use() override;

private:
    const VertexArray* verticesArray;
    const IndexBuffer* indexBuffer;
    GLuint programID;
};

}  // namespace Marbas

#endif
