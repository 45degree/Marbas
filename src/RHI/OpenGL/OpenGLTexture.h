#ifndef MARBARS_RHI_OPENGL_TEXTURE_H
#define MARBARS_RHI_OPENGL_TEXTURE_H

#include "RHI/OpenGL/OpenGLRHICommon.h"
#include "RHI/Interface/Texture.h"

namespace Marbas {

class OpenGLTexture2D : public Texture2D {
public:
    OpenGLTexture2D(int width, int height, TextureFormatType format);
    ~OpenGLTexture2D() override;

public:
    void Bind(int uniformBind) override;
    void SetData(void* data, uint32_t size) override;
    void UnBind() override;
    void* GetTexture() override;

private:
    GLuint textureID;
};

}  // namespace Marbas

#endif
