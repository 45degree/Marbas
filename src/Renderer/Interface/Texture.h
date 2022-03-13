#ifndef MARBARS_RENDERER_INTERFACE_TEXTURE_H
#define MARBARS_RENDERER_INTERFACE_TEXTURE_H

#include "Common.h"

namespace Marbas {

class Texture2D {
public:
    explicit Texture2D(const FileSystem::path& imagePath) {}
    Texture2D(int width, int height) : width(width), height(height) {}
    virtual ~Texture2D() = default;

    virtual void Bind(int uniformBind) = 0;
    virtual void SetData(void* data, uint32_t size) = 0;
    virtual void UnBind() = 0;

protected:
    int width;
    int height;

};

}  // namespace Marbas

#endif
