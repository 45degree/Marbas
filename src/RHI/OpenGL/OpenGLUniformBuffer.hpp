#ifndef MARBAS_RHI_OPENGL_UNIFORMBUFFER_H
#define MARBAS_RHI_OPENGL_UNIFORMBUFFER_H

#include "Common.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"
#include "RHI/Interface/UniformBuffer.hpp"

namespace Marbas {

class OpenGLUniformBuffer : public UniformBuffer {
public:
    explicit OpenGLUniformBuffer(uint32_t size, uint32_t bindingPoint);
    ~OpenGLUniformBuffer() override;

public:
    void Bind() const override;

    void UnBind() const override;

    void SetData(const void* data, uint32_t size, uint32_t offset) override;

private:
    GLuint UBO;
};

}  // namespace Marbas

#endif
