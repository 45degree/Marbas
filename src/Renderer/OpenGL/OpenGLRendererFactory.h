#ifndef MARBAS_RENDERER_OPENGL_RENDERER_FACTORY_H
#define MARBAS_RENDERER_OPENGL_RENDERER_FACTORY_H

#include "Renderer/Interface/RendererFactory.h"
#include "Renderer/OpenGL/OpenGLTexture.h"

#include <unordered_map>

namespace Marbas {

class OpenGLRendererFactory : public RendererFactory {
private:
    OpenGLRendererFactory() = default;

public:
    OpenGLRendererFactory(const OpenGLRendererFactory&) = delete;
    OpenGLRendererFactory& operator=(const OpenGLRendererFactory&) = delete;
    ~OpenGLRendererFactory() override = default;

public:

    [[nodiscard]] std::unique_ptr<VertexBuffer>
    CreateVertexBuffer(const Vector<float>& data) const override;

    [[nodiscard]] std::unique_ptr<VertexArray>
    CreateVertexArray() const override;

    [[nodiscard]] std::unique_ptr<IndexBuffer>
    CreateIndexBuffer(const Vector<uint32_t>& indices) const override;

    [[nodiscard]] std::unique_ptr<ShaderCode>
    CreateShaderCode(const Path& path, const ShaderCodeType type,
                     const ShaderType shaderType) const override;

    [[nodiscard]] std::unique_ptr<Shader>
    CreateShader() const override;

    [[nodiscard]] Texture2D*
    CreateTexutre2D(const Path& imagePath) override;

    [[nodiscard]] std::unique_ptr<DrawCollection>
    CreateDrawCollection() const override;

    [[nodiscard]] std::unique_ptr<UniformBuffer>
    CreateUniformBuffer(uint32_t size, uint32_t bindingPoint) const override;

    [[nodiscard]] std::unique_ptr<FrameBuffer>
    CreateFrameBuffer(const FrameBufferInfo& info) const override;

    [[nodiscard]] std::unique_ptr<Viewport>
    CreateViewport() const override;

private:
    std::unordered_map<String, std::unique_ptr<OpenGLTexture2D>> m_Textures2D;

    friend RendererFactory;
};

}  // namespace Marbas

#endif
