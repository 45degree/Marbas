#ifndef MARBARS_RENDER_INTERFACE_SHADER_H
#define MARBARS_RENDER_INTERFACE_SHADER_H

#include "Renderer/Interface/ShaderCode.h"
#include "Renderer/Interface/VertexBuffer.h"
#include "Renderer/Interface/VertexArray.h"
#include "Renderer/Interface/IndexBuffer.h"
#include "Renderer/Interface/Texture.h"

namespace Marbas {

class Shader {
public:
    Shader() = default;
    virtual ~Shader() = default;

public:

    // virtual void AddIndeices(const IndexBuffer* indices) = 0;

    virtual void AddShaderCode(const ShaderCode* shaderCode) = 0;

    virtual void AddUniformDataBlock(uint32_t bindingPoint, const void* data, uint32_t size) = 0;

    virtual void Link() = 0;

    // virtual void Draw() = 0;

    virtual void Use() = 0;
};

}  // namespace Marbas

#endif
