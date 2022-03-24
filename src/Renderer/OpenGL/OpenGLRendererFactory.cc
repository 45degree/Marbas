#include "Renderer/OpenGL/OpenGLRendererFactory.h"
#include "Renderer/OpenGL/OpenGLDrawCollection.h"
#include "Renderer/OpenGL/OpenGLFrameBuffer.h"
#include "Renderer/OpenGL/OpenGLIndexBuffer.h"
#include "Renderer/OpenGL/OpenGLShader.h"
#include "Renderer/OpenGL/OpenGLVertexBuffer.h"
#include "Renderer/OpenGL/OpenGLVertexArray.h"
#include "Renderer/OpenGL/OpenGLShaderCode.h"
#include "Renderer/OpenGL/OpenGLViewport.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Marbas {

std::unique_ptr<FrameBuffer>
OpenGLRendererFactory::CreateFrameBuffer(const FrameBufferInfo& info) const {
    return std::make_unique<OpenGLFrameBuffer>(info);
}

std::unique_ptr<VertexBuffer>
OpenGLRendererFactory::CreateVertexBuffer(const Vector<float>& data) const {
    return std::make_unique<OpenGLVertexBuffer>(data);
}

std::unique_ptr<VertexArray>
OpenGLRendererFactory::CreateVertexArray() const {
    return std::make_unique<OpenGLVertexArray>();
}

std::unique_ptr<IndexBuffer>
OpenGLRendererFactory::CreateIndexBuffer(const Vector<uint32_t>& indices) const {
    return std::make_unique<OpenGLIndexBuffer>(indices);
}

std::unique_ptr<ShaderCode>
OpenGLRendererFactory::CreateShaderCode(const Path& path, const ShaderCodeType type,
                                        const ShaderType shaderType) const {
    auto shaderCode = std::make_unique<OpenGLShaderCode>(shaderType);
    if(type == ShaderCodeType::FILE) {
        shaderCode->ReadFromSource(path);
    }
    else if (type == ShaderCodeType::BINARY) {
        shaderCode->ReadSPIR_V(path, "main");
    }

    return shaderCode;
}

std::unique_ptr<Shader>
OpenGLRendererFactory::CreateShader() const {
    return std::make_unique<OpenGLShader>();
}

Texture2D*
OpenGLRendererFactory::CreateTexutre2D(const Path& imagePath) {
    String pathStr = String(imagePath.string());

    if(m_Texture2DImages.find(pathStr) != m_Texture2DImages.end()) {
        return m_Texture2DImages.at(pathStr).get();
    }

    // load image
    int width, height, nrChannels;
    TextureFormatType formatType;

    // tell stb_image.h to flip loaded texture's on the y-axis.
    stbi_set_flip_vertically_on_load(true);
    auto filename = imagePath.string();
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    if(data == nullptr) {
        LOG(ERROR) << FORMAT("failed to load image from {}", filename);
        return nullptr;
    }

    formatType = nrChannels == 4 ? TextureFormatType::RGBA : TextureFormatType::RGB;

    auto dataFormat = formatType == TextureFormatType::RGBA ? GL_RGBA : GL_RGB;
    auto internalFormat = formatType == TextureFormatType::RGBA ? GL_RGBA8 : GL_RGB8;

    // TODO(45degree): calculate hash for this image

    // create textrue
    auto texture = std::make_unique<OpenGLTexture2D>(width, height, formatType);
    texture->SetData(data, width * height * nrChannels);

    stbi_image_free(data);

    LOG(INFO) << FORMAT("create a opengl texture, the image is {}", pathStr);

    m_Texture2DImages[pathStr] = std::move(texture);
    return m_Texture2DImages[pathStr].get();
}

Texture2D*
OpenGLRendererFactory::CreateTexutre2D(int width, int height, TextureFormatType format) {
    m_Texture2DDynamic.push_back(std::make_unique<OpenGLTexture2D>(width, height, format));
    auto size = m_Texture2DDynamic.size();
    return m_Texture2DDynamic[size - 1].get();
}


void OpenGLRendererFactory::DestoryTexture2D(Texture2D* texture) {
    // TODO
}


std::unique_ptr<DrawCollection>
OpenGLRendererFactory::CreateDrawCollection() const {
    return std::make_unique<OpenGLDrawCollection>();
}

std::unique_ptr<UniformBuffer>
OpenGLRendererFactory::CreateUniformBuffer(uint32_t size, uint32_t bindingPoint) const {
    return std::make_unique<OpenGLUniformBuffer>(size, bindingPoint);
}

std::unique_ptr<Viewport>
OpenGLRendererFactory::CreateViewport() const {
    return std::make_unique<OpenGLViewport>();
}

}  // namespace Marbas
