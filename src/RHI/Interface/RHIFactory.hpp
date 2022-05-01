#ifndef MARBAS_RHI_INTERFACE_RENDERER_FACETORY_H
#define MARBAS_RHI_INTERFACE_RENDERER_FACETORY_H

#include "RHI/Interface/DrawBatch.hpp"
#include "RHI/Interface/FrameBuffer.hpp"
#include "RHI/Interface/Shader.hpp"
#include "RHI/Interface/UniformBuffer.hpp"
#include "RHI/Interface/VertexArray.hpp"
#include "RHI/Interface/VertexBuffer.hpp"
#include "RHI/Interface/ShaderCode.hpp"
#include "RHI/Interface/Viewport.hpp"

#include <GLFW/glfw3.h>
#include <memory>

namespace Marbas {

enum class RendererType {
    OPENGL,
    VULKAN,
};

enum class ShaderCodeType {
    FILE,
    BINARY,
};

enum class ClearBuferBit {
    COLOR_BUFFER,
    DEPTH_BUFFER,
};

enum class EnableItem {
    DEPTH,
    BLEND,
};

/**
 * @brief 
 */
class RHIFactory {
protected:
    RHIFactory() = default;

public:
    RHIFactory(const RHIFactory&) = delete;
    RHIFactory& operator=(const RHIFactory&) = delete;

    virtual ~RHIFactory() = default;

public:

    virtual void Enable(EnableItem item) const = 0;

    virtual void Disable(EnableItem item) const = 0;

    virtual void ClearColor(float r, float g, float b, float a) const = 0;

    virtual void PrintRHIInfo() const = 0;

    virtual void Init(GLFWwindow* window) const = 0;

    virtual void SwapBuffer(GLFWwindow* window) const = 0;

    virtual void ClearBuffer(const ClearBuferBit bufferBit) const = 0;

    [[nodiscard]] virtual std::unique_ptr<VertexBuffer>
    CreateVertexBuffer(const void* data, size_t size) const = 0;

    [[nodiscard]] virtual std::unique_ptr<VertexBuffer>
    CreateVertexBuffer(size_t size) const = 0;

    [[nodiscard]] virtual std::unique_ptr<VertexArray>
    CreateVertexArray() const = 0;

    [[nodiscard]] virtual std::unique_ptr<IndexBuffer>
    CreateIndexBuffer(const Vector<uint32_t>& indices) const = 0;

    [[nodiscard]] virtual std::unique_ptr<IndexBuffer>
    CreateIndexBuffer(size_t size) const = 0;

    [[nodiscard]] virtual std::unique_ptr<ShaderCode>
    CreateShaderCode(const Path& path, const ShaderCodeType codeType,
                     const ShaderType shaderType) const = 0;

    [[nodiscard]] virtual std::unique_ptr<Shader>
    CreateShader() const = 0;

    [[nodiscard]] virtual std::unique_ptr<Texture2D>
    CreateTexutre2D(const Path& imagePath) = 0;

    [[nodiscard]] virtual std::unique_ptr<Texture2D>
    CreateTexutre2D(int width, int height, TextureFormatType formatType) = 0;

    // virtual void DestoryTexture2D(Texture2D* texture) = 0;

    [[nodiscard]] virtual std::unique_ptr<DrawBatch>
    CreateDrawBatch() const = 0;

    [[nodiscard]] virtual std::unique_ptr<UniformBuffer>
    CreateUniformBuffer(uint32_t size, uint32_t bindingPoint) const = 0;

    [[nodiscard]] virtual std::unique_ptr<FrameBuffer>
    CreateFrameBuffer(const FrameBufferInfo& info) const = 0;

    [[nodiscard]] virtual std::unique_ptr<Viewport>
    CreateViewport() const = 0;

public:
    static RHIFactory* GetInstance(const RendererType& rendererType);

private:
    static std::unique_ptr<RHIFactory> m_rhiFactory;
};

}  // namespace Marbas


#endif
