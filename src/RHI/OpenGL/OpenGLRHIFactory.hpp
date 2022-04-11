#ifndef MARBAS_RHI_OPENGL_RENDERER_FACTORY_H
#define MARBAS_RHI_OPENGL_RENDERER_FACTORY_H

#include "RHI/Interface/RHIFactory.hpp"
#include "RHI/OpenGL/OpenGLTexture.hpp"

#include <unordered_map>

namespace Marbas {

class OpenGLRHIFactory : public RHIFactory {
private:
    OpenGLRHIFactory();

public:
    OpenGLRHIFactory(const OpenGLRHIFactory&) = delete;
    OpenGLRHIFactory& operator=(const OpenGLRHIFactory&) = delete;
    ~OpenGLRHIFactory() override = default;

public:

    void Enable(EnableItem item) const override;

    void Disable(EnableItem item) const override;

    void ClearColor(float r, float g, float b, float a) const override;

    void PrintRHIInfo() const override;

    void Init(GLFWwindow *window) const override;

    void SwapBuffer(GLFWwindow* window) const override;

    void ClearBuffer(const ClearBuferBit bufferBit) const override;

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

    [[nodiscard]] Texture2D*
    CreateTexutre2D(int width, int height, TextureFormatType formatType) override;

    void DestoryTexture2D(Texture2D* texture) override;

    [[nodiscard]] std::unique_ptr<DrawCollection>
    CreateDrawCollection() const override;

    [[nodiscard]] std::unique_ptr<UniformBuffer>
    CreateUniformBuffer(uint32_t size, uint32_t bindingPoint) const override;

    [[nodiscard]] std::unique_ptr<FrameBuffer>
    CreateFrameBuffer(const FrameBufferInfo& info) const override;

    [[nodiscard]] std::unique_ptr<Viewport>
    CreateViewport() const override;

private:
    OpenGLTexturePool m_texturePool;

    friend RHIFactory;
};

}  // namespace Marbas

#endif
