#include "Renderer/OpenGL/OpenGLTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Marbas {

OpenGLTexture2D::OpenGLTexture2D(const FileSystem::path& imagePath): Texture2D(imagePath) {
    // load image
    int width, height, nrChannels;

    // tell stb_image.h to flip loaded texture's on the y-axis.
    stbi_set_flip_vertically_on_load(true);
    auto filename = imagePath.string();
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    if(data == nullptr) {
        LOG(ERROR) << FORMAT("failed to load image from {}", filename);
        return;
    }

    this->width = width;
    this->height = height;
    dataFormat = nrChannels == 4 ? GL_RGBA : GL_RGB;
    internalFormat = nrChannels == 4 ? GL_RGBA8 : GL_RGB8;

    // create textrue
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glTextureStorage2D(textureID, 1, internalFormat, width, height);

    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureSubImage2D(textureID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D); 

    LOG(INFO) << "texture error: " << glGetError();

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGLTexture2D::OpenGLTexture2D(int width, int height) : Texture2D(width, height) {
    internalFormat = GL_RGBA8;
    dataFormat = GL_RGBA;

    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glTextureStorage2D(textureID, 1, internalFormat, width, height);

    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void OpenGLTexture2D::Bind(int uniformBind) {
    // glActiveTexture(GL_TEXTURE0 + uniformBind);
    // glBindTexture(GL_TEXTURE_2D, textureID);
    glBindTextureUnit(uniformBind, textureID);
    LOG(INFO) << "texture error: " << glGetError();
}

void OpenGLTexture2D::SetData(void *data, uint32_t size) {
    auto bpp = dataFormat == GL_RGBA ? 4 : 3;
    LOG_IF(ERROR, size != width * height * bpp) << "size and texture size do not match";

    glTextureSubImage2D(textureID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_INT, data);
}

void OpenGLTexture2D::UnBind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGLTexture2D::~OpenGLTexture2D() {
    glDeleteTextures(1, &textureID);
}

}  // namespace Marbas
