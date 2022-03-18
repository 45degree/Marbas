#ifndef MARBARS_RENDERER_OPENGL_INDEX_BUFFER_H
#define MARBARS_RENDERER_OPENGL_INDEX_BUFFER_H

#include "Renderer/Interface/IndexBuffer.h"

namespace Marbas {

class OpenGLIndexBuffer : public IndexBuffer {
public:
    explicit OpenGLIndexBuffer(size_t size);
    explicit OpenGLIndexBuffer(const Vector<uint32_t>& data);
    ~OpenGLIndexBuffer() override;

public:
    void SetData(const Vector<uint32_t>& data) override;
    void Bind() const override;
    void UnBind() const override;

private:
    unsigned int EBO;
};

}  // namespace Marbas

#endif
