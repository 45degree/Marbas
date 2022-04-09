#include "RHI/OpenGL/OpenGLTexture.hpp"

#include <glog/logging.h>

namespace Marbas {

static GLenum ConvertToOpenglDataFormat(TextureFormatType type) {
    switch(type) {
    case TextureFormatType::BGR: return GL_BGR;
    case TextureFormatType::BGRA: return GL_BGRA;
    case TextureFormatType::RED: return GL_RED;
    case TextureFormatType::RG: return GL_RG;
    case TextureFormatType::RGB: return GL_RGB;
    case TextureFormatType::RGBA: return GL_RGBA;
    }
}

static GLenum ConvertToOpenglInternalFormat(TextureFormatType type) {
    switch(type) {
    case TextureFormatType::BGR: return GL_RGB8;
    case TextureFormatType::BGRA: return GL_RGBA8;
    case TextureFormatType::RED: return GL_R8;
    case TextureFormatType::RG: return GL_RG8;
    case TextureFormatType::RGB: return GL_RGB8;
    case TextureFormatType::RGBA: return GL_RGBA8;
    }
}

OpenGLTexture2D::OpenGLTexture2D(int width, int height, TextureFormatType format) :
    Texture2D(width, height, format)
{

    auto dataFormat = ConvertToOpenglDataFormat(format);
    auto internalFormat = ConvertToOpenglInternalFormat(format);

    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glTextureStorage2D(textureID, 1, internalFormat, width, height);

    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void OpenGLTexture2D::Bind(int uniformBind) {
    glBindTextureUnit(uniformBind, textureID);

    auto error = glGetError();
    LOG_IF(ERROR, error) << FORMAT("can't bind texture {} to {}, error code is {}",
                                   textureID, uniformBind, error);
}

void OpenGLTexture2D::SetData(void *data, uint32_t size) {
    auto bpp = this->format == TextureFormatType::RGBA ? 4 : 3;
    LOG_IF(ERROR, size != width * height * bpp) << "size and texture size do not match";

    auto dataFormat = ConvertToOpenglDataFormat(this->format);
    glTextureSubImage2D(textureID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

    auto error = glGetError();
    LOG_IF(ERROR, error) << FORMAT("can't submit the data for texture {}, error code is {}",
                                   textureID, error);
}

void OpenGLTexture2D::UnBind() {
    glBindTexture(GL_TEXTURE_2D, 0);

    auto error = glGetError();
    LOG_IF(ERROR, error) << FORMAT("can't unbind texture: {}, error code is {}", textureID, error);
}


void* OpenGLTexture2D::GetTexture() {
    return reinterpret_cast<void*>(textureID);
}

OpenGLTexture2D::~OpenGLTexture2D() {
    glDeleteTextures(1, &textureID);
}

}  // namespace Marbas
