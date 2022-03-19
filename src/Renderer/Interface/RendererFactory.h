#ifndef MARBAS_RENDERER_INTERFACE_RENDERER_FACETORY_H
#define MARBAS_RENDERER_INTERFACE_RENDERER_FACETORY_H

#include "Renderer/Interface/DrawCollection.h"
#include "Renderer/Interface/FrameBuffer.h"
#include "Renderer/Interface/Shader.h"
#include "Renderer/Interface/UniformBuffer.h"
#include "Renderer/Interface/VertexArray.h"
#include "Renderer/Interface/VertexBuffer.h"
#include "Renderer/Interface/ShaderCode.h"
#include "Renderer/Interface/Viewport.h"

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
class RendererFactory {
protected:
    RendererFactory() = default;

public:
    RendererFactory(const RendererFactory&) = delete;
    RendererFactory& operator=(const RendererFactory&) = delete;

    virtual ~RendererFactory() = default;

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

    [[nodiscard]] virtual std::unique_ptr<DrawCollection>
    CreateDrawCollection() const = 0;

    [[nodiscard]] virtual std::unique_ptr<UniformBuffer>
    CreateUniformBuffer(uint32_t size, uint32_t bindingPoint) const = 0;

    [[nodiscard]] virtual std::unique_ptr<FrameBuffer>
    CreateFrameBuffer(const FrameBufferInfo& info) const = 0;

    [[nodiscard]] virtual std::unique_ptr<Viewport>
    CreateViewport() const = 0;

public:
    static RendererFactory* GetInstance(const RendererType& rendererType);

private:
    static std::unique_ptr<RendererFactory> m_rendererFactory;
};

}  // namespace Marbas


#endif
