#ifndef MARBARS_RENDERER_INTERFACE_TEXTURE_H
#define MARBARS_RENDERER_INTERFACE_TEXTURE_H

#include "Common.h"

namespace Marbas {

enum class TextureFormatType {
    RED,
    RG,
    RGB,
    BGR,
    RGBA,
    BGRA,
};

class Texture2D {
public:
    Texture2D(int width, int height, TextureFormatType formatType) :
        width(width), height(height), format(formatType)
    {}

    virtual ~Texture2D() = default;

    virtual void Bind(int uniformBind) = 0;
    virtual void SetData(void* data, uint32_t size) = 0;
    virtual void UnBind() = 0;
    virtual void* GetTexture() = 0;

protected:
    // String m_imagePath;
    int width;
    int height;
    TextureFormatType format;
};

class Texture2DPool {
public:
    Texture2DPool() = default;
    ~Texture2DPool() = default;
};

}  // namespace Marbas

#endif
