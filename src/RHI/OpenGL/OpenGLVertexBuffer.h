#ifndef MARBARS_RHI_OPENGL_VERTEX_BUFFER_H
#define MARBARS_RHI_OPENGL_VERTEX_BUFFER_H

#include "RHI/Interface/VertexBuffer.h"
#include "Common.h"

namespace Marbas {

class OpenGLVertexBuffer : public VertexBuffer {
public:
    explicit OpenGLVertexBuffer(std::size_t size);
    explicit OpenGLVertexBuffer(const Vector<float>& data);

    ~OpenGLVertexBuffer() override;

public:
    void Bind() const override;
    void UnBind() const override;
    void SetData(const Vector<float>& data) const override;

private:
    GLuint VBO;
};

}  // namespace Marbas

#endif
