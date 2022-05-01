#ifndef MARBARS_RHI_INTERFACE_TEXTURE_H
#define MARBARS_RHI_INTERFACE_TEXTURE_H

#include "Common.hpp"
#include <optional>

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
    Texture2D(int width, int height, TextureFormatType formatType):
        width(width),
        height(height),
        format(formatType)
    {}

    virtual ~Texture2D() = default;
    virtual void Bind(int uniformBind) = 0;
    virtual void SetData(void* data, uint32_t size) = 0;
    virtual void UnBind() = 0;
    virtual void* GetTexture() = 0;

    void SetImageInfo(const String& imagePath, uint32_t hashCode) {
        m_imagePath = imagePath;
        m_hashCode = hashCode;
    }

    void SetTextureId(uint32_t textureId) noexcept {
        m_textureId = textureId;
    }

    [[nodiscard]] uint32_t GetTextureId() const noexcept {
        return m_textureId;
    }

    [[nodiscard]] String GetImagePath() const {
        return m_imagePath;
    }

    [[nodiscard]] bool IsImageTexture() const noexcept {
        return m_hashCode.has_value();
    }

    [[nodiscard]] uint32_t GetHashCode() const noexcept {
        if(m_hashCode.has_value()) return m_hashCode.value();
        return 0;
    }

protected:
    uint32_t m_textureId = 0;
    std::optional<uint32_t> m_hashCode = std::nullopt;
    String m_imagePath;

    int width;
    int height;
    TextureFormatType format;
};

}  // namespace Marbas

#endif
