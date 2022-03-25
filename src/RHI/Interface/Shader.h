#ifndef MARBARS_RHI_INTERFACE_SHADER_H
#define MARBARS_RHI_INTERFACE_SHADER_H

#include "RHI/Interface/ShaderCode.h"
#include "RHI/Interface/VertexBuffer.h"
#include "RHI/Interface/VertexArray.h"
#include "RHI/Interface/IndexBuffer.h"
#include "RHI/Interface/Texture.h"

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
