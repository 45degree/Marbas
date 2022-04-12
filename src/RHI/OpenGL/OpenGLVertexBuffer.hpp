#ifndef MARBARS_RHI_OPENGL_VERTEX_BUFFER_H
#define MARBARS_RHI_OPENGL_VERTEX_BUFFER_H

#include "RHI/Interface/VertexBuffer.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"
#include "Common.hpp"

namespace Marbas {

class OpenGLVertexBuffer : public VertexBuffer {
public:
    explicit OpenGLVertexBuffer(std::size_t size);
    explicit OpenGLVertexBuffer(const void* data, size_t size);

    ~OpenGLVertexBuffer() override;

public:
    void Bind() const override;
    void UnBind() const override;
    void SetData(const void* data, size_t size, size_t offset) const override;

private:
    GLuint VBO;
};

}  // namespace Marbas

#endif
