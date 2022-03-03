#ifndef MARBARS_RENDERER_INTERFACE_FRAMEBUFFER_H
#define MARBARS_RENDERER_INTERFACE_FRAMEBUFFER_H

namespace Marbas {

struct FrameBufferInfo {
    int width;
    int height;
    bool depthAttach;
    bool templateAttach;
};

class FrameBuffer {
public:
    explicit FrameBuffer(const FrameBufferInfo& info) : frameBufferInfo(info) {};
    virtual ~FrameBuffer() = default;

public:
    virtual void Bind() const = 0;
    virtual void UnBind() const = 0;

    [[nodiscard]] virtual const void* GetColorAttachTexture() const = 0;

protected:
    FrameBufferInfo frameBufferInfo;
};

}  // namespace Marbas

#endif
