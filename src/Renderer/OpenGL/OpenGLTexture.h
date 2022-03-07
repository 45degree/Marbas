#ifndef MARBARS_RENDERER_OPENGL_TEXTURE_H
#define MARBARS_RENDERER_OPENGL_TEXTURE_H

#include "Renderer/Interface/Texture.h"

namespace Marbas {

class OpenGLTexture2D : public Texture2D {
public:
    explicit OpenGLTexture2D(const String& textureName);
    ~OpenGLTexture2D() override;

public:
    void ReadImage(const FileSystem::path& path) override;
    void Bind(int uniformBind) override;
    void UnBind() override;

private:
    GLuint textureID;
};

}  // namespace Marbas

#endif
