#include "Renderer/OpenGL/OpenGLRendererFactory.h"
#include "Renderer/OpenGL/OpenGLDrawCollection.h"
#include "Renderer/OpenGL/OpenGLFrameBuffer.h"
#include "Renderer/OpenGL/OpenGLIndexBuffer.h"
#include "Renderer/OpenGL/OpenGLShader.h"
#include "Renderer/OpenGL/OpenGLVertexBuffer.h"
#include "Renderer/OpenGL/OpenGLVertexArray.h"
#include "Renderer/OpenGL/OpenGLShaderCode.h"
#include "Renderer/OpenGL/OpenGLViewport.h"

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
    if(m_Textures2D.find(pathStr) == m_Textures2D.end()) {
        m_Textures2D[pathStr] = std::make_unique<OpenGLTexture2D>(imagePath);
    }
    return m_Textures2D[pathStr].get();
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
