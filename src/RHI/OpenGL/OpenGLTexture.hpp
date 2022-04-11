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

class OpenGLTexturePool {
public:
    OpenGLTexturePool() = default;
    ~OpenGLTexturePool() = default;
public:
    void AddTexture(std::unique_ptr<OpenGLTexture2D>&& texture,
                     std::optional<uint32_t> hashCode = std::nullopt);

    [[nodiscard]] OpenGLTexture2D* GetTextureByHashCode(uint32_t hashCode) const {
        if(m_imageTextures.find(hashCode) != m_imageTextures.end()) {
            return m_imageTextures.at(hashCode);
        }
        return nullptr;
    }

    [[nodiscard]] OpenGLTexture2D* GetTextureByID(uint32_t textureId) const noexcept {
        if(textureId >= m_textures.size()) return nullptr;
        return m_textures[textureId].get();
    }

    [[nodiscard]] bool IsImageTexutreExisted(uint32_t hashCode) const noexcept {
        return m_imageTextures.find(hashCode) != m_imageTextures.end();
    }

    void DeleteTexture(OpenGLTexture2D* texture);

private:
    Vector<std::unique_ptr<OpenGLTexture2D>> m_textures;
    std::unordered_map<uint32_t, OpenGLTexture2D*> m_imageTextures;
};

}  // namespace Marbas

#endif
