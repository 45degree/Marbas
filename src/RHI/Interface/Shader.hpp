#ifndef MARBARS_RHI_INTERFACE_SHADER_H
#define MARBARS_RHI_INTERFACE_SHADER_H

#include "RHI/Interface/ShaderCode.hpp"
#include "RHI/Interface/VertexBuffer.hpp"
#include "RHI/Interface/VertexArray.hpp"
#include "RHI/Interface/IndexBuffer.hpp"
#include "RHI/Interface/Texture.hpp"

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
