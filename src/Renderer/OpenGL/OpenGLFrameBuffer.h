#ifndef MARBARS_RENDERER_OPENGL_FRAMEBUFFER_H
#define MARBARS_RENDERER_OPENGL_FRAMEBUFFER_H

#include "Renderer/Interface/FrameBuffer.h"
#include "Common.h"

namespace Marbas {

class OpenGLFrameBuffer : public FrameBuffer {
public:
    explicit OpenGLFrameBuffer(const FrameBufferInfo& info);
    ~OpenGLFrameBuffer() override;

public:
    void Create();

    void Bind() const override;
    void UnBind() const override;

    [[nodiscard]] const void* GetColorAttachTexture() const noexcept override {
        return reinterpret_cast<const void*>(colorAttachTexture);
    }

    void ChangeBufferSize(int width, int height);

private:
    GLuint frameBufferID;
    GLuint colorAttachTexture;
};

}  // namespace Marbas

#endif
