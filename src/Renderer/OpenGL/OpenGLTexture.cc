#include "Renderer/OpenGL/OpenGLTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Marbas {

OpenGLTexture2D::OpenGLTexture2D(const String& textureName) : Texture2D(textureName) {
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
     // set the texture wrapping parameters

    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void OpenGLTexture2D::Bind(int uniformBind) {
    glActiveTexture(GL_TEXTURE0 + uniformBind);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void OpenGLTexture2D::UnBind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLTexture2D::ReadImage(const FileSystem::path &path) {
    int width, height, nrChannels;

    // tell stb_image.h to flip loaded texture's on the y-axis.
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if(data == nullptr) {
        LOG(ERROR) << FORMAT("failed to load image from {}", path.c_str());
        return;
    }

    GLenum imageFormat = nrChannels == 4 ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D() {
    glDeleteTextures(1, &textureID);
}

}  // namespace Marbas
