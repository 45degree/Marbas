#ifndef MARBARS_RHI_OPENGL_INDEX_BUFFER_H
#define MARBARS_RHI_OPENGL_INDEX_BUFFER_H

#include "RHI/Interface/IndexBuffer.hpp"

namespace Marbas {

class OpenGLIndexBuffer : public IndexBuffer {
public:
    explicit OpenGLIndexBuffer(size_t size);
    explicit OpenGLIndexBuffer(const Vector<uint32_t>& data);
    ~OpenGLIndexBuffer() override;

public:
    void SetData(const Vector<uint32_t>& data, size_t offset) override;
    void Bind() const override;
    void UnBind() const override;

private:
    unsigned int EBO;
};

}  // namespace Marbas

#endif
