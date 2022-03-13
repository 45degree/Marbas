#ifndef MARBARS_RENDERER_OPENGL_TEXTURE_H
#define MARBARS_RENDERER_OPENGL_TEXTURE_H

#include "Renderer/Interface/Texture.h"

namespace Marbas {

class OpenGLTexture2D : public Texture2D {
public:
    explicit OpenGLTexture2D(const FileSystem::path& imagePath);
    OpenGLTexture2D(int width, int height);
    ~OpenGLTexture2D() override;

public:
    void Bind(int uniformBind) override;
    void SetData(void* data, uint32_t size) override;
    void UnBind() override;

private:
    GLuint textureID;
    GLenum internalFormat, dataFormat;
};

}  // namespace Marbas

#endif
