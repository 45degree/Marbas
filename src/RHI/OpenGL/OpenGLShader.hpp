#ifndef MARBAS_RHI_OPENGL_SHADER_H
#define MARBAS_RHI_OPENGL_SHADER_H

#include "RHI/Interface/Shader.hpp"
#include "RHI/Interface/IndexBuffer.hpp"
#include "RHI/OpenGL/OpenGLTexture.hpp"
#include "RHI/OpenGL/OpenGLUniformBuffer.hpp"

namespace Marbas {

class OpenGLShader : public Shader{
public:
    OpenGLShader();
    ~OpenGLShader() override;

public:

    void AddShaderCode(const ShaderCode* shaderCode) override;

    void Link() override;

    void Use() const override;

    void AddUniformDataBlock(uint32_t bindingPoint,  const void* data, uint32_t size) override;

private:
    GLuint programID;

    bool m_isLink = false;
    std::unordered_map<uint32_t, std::unique_ptr<OpenGLUniformBuffer>> m_uniformDataBlocks;
};

}  // namespace Marbas

#endif
