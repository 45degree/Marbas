#ifndef MARBARS_RHI_OPENGL_VERTEX_ARRAY_H
#define MARBARS_RHI_OPENGL_VERTEX_ARRAY_H

#include "Common.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"
#include "RHI/Interface/VertexArray.hpp"

namespace Marbas {

class OpenGLVertexArray : public VertexArray {
public:
    OpenGLVertexArray();
    ~OpenGLVertexArray() override;

public:
    void Bind() const override;
    void UnBind() const override;

    void EnableVertexAttribArray(const VertexBuffer* vertexBuffer) const override;
private:
    GLuint VAO;
};

}  // namespace Marbas

#endif