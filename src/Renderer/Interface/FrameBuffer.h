#ifndef MARBARS_RENDERER_INTERFACE_FRAMEBUFFER_H
#define MARBARS_RENDERER_INTERFACE_FRAMEBUFFER_H

namespace Marbas {

struct FrameBufferInfo {
    int width = 0;
    int height = 0;
    bool depthAttach = false;
    bool templateAttach = false;
};

class FrameBuffer {
public:
    explicit FrameBuffer(const FrameBufferInfo& info) : frameBufferInfo(info) {};
    virtual ~FrameBuffer() = default;

public:
    virtual void Bind() const = 0;
    virtual void UnBind() const = 0;

    virtual void Create() = 0;
    virtual void ReCreate() = 0;
    virtual void Resize(int width, int height) = 0;

    [[nodiscard]] virtual const void* GetColorAttachTexture() const = 0;

protected:
    FrameBufferInfo frameBufferInfo;
};

}  // namespace Marbas

#endif
