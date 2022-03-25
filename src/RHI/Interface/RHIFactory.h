#ifndef MARBAS_RHI_INTERFACE_RENDERER_FACETORY_H
#define MARBAS_RHI_INTERFACE_RENDERER_FACETORY_H

#include "RHI/Interface/DrawCollection.h"
#include "RHI/Interface/FrameBuffer.h"
#include "RHI/Interface/Shader.h"
#include "RHI/Interface/UniformBuffer.h"
#include "RHI/Interface/VertexArray.h"
#include "RHI/Interface/VertexBuffer.h"
#include "RHI/Interface/ShaderCode.h"
#include "RHI/Interface/Viewport.h"

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

    [[nodiscard]] virtual std::unique_ptr<VertexBuffer>
    CreateVertexBuffer(const Vector<float>& data) const = 0;

    [[nodiscard]] virtual std::unique_ptr<VertexArray>
    CreateVertexArray() const = 0;

    [[nodiscard]] virtual std::unique_ptr<IndexBuffer>
    CreateIndexBuffer(const Vector<uint32_t>& indices) const = 0;

    [[nodiscard]] virtual std::unique_ptr<ShaderCode>
    CreateShaderCode(const Path& path, const ShaderCodeType codeType,
                     const ShaderType shaderType) const = 0;

    [[nodiscard]] virtual std::unique_ptr<Shader>
    CreateShader() const = 0;

    [[nodiscard]] virtual Texture2D*
    CreateTexutre2D(const Path& imagePath) = 0;

    [[nodiscard]] virtual Texture2D*
    CreateTexutre2D(int width, int height, TextureFormatType formatType) = 0;

    virtual void DestoryTexture2D(Texture2D* texture) = 0;

    [[nodiscard]] virtual std::unique_ptr<DrawCollection>
    CreateDrawCollection() const = 0;

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
