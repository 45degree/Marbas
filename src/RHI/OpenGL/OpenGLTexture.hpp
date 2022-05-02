#ifndef MARBARS_RHI_OPENGL_TEXTURE_H
#define MARBARS_RHI_OPENGL_TEXTURE_H

#include "RHI/OpenGL/OpenGLRHICommon.hpp"
#include "RHI/Interface/Texture.hpp"

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

class OpenGLTextureCubeMap : public TextureCubeMap {
public:
    explicit OpenGLTextureCubeMap(int width, int height, TextureFormatType format);

    ~OpenGLTextureCubeMap() override = default;

public:
    void Bind(int bindingPoint) override;
    void SetData(void* data, uint32_t size, CubeMapPosition position) override;
    void UnBind() override;

private:
    GLuint m_textureID;
};

}  // namespace Marbas

#endif
