#ifndef MARBAS_RHI_OPENGL_SHADER_H
#define MARBAS_RHI_OPENGL_SHADER_H

#include "RHI/Interface/Shader.h"
#include "RHI/Interface/IndexBuffer.h"
#include "RHI/OpenGL/OpenGLTexture.h"
#include "RHI/OpenGL/OpenGLUniformBuffer.h"

namespace Marbas {

class OpenGLShader : public Shader{
public:
    OpenGLShader();
    ~OpenGLShader() override;

public:

    // void AddVertices(const VertexBuffer* vertexBuffer, const VertexArray* verticesArray) override;

    // void AddIndeices(const IndexBuffer* indices) override;

    void AddShaderCode(const ShaderCode* shaderCode) override;

    void Link() override;

    // void Draw() override;

    void Use() override;

    void AddUniformDataBlock(uint32_t bindingPoint,  const void* data, uint32_t size) override;

    // void AddTexture(Texture2D* texture, int uniformBind) override;

    // void SetMvp(const glm::mat4& model, const glm::mat4& view,
    //             const glm::mat4& projection) override;

private:
    // const VertexArray* verticesArray;
    // const IndexBuffer* indexBuffer;
    GLuint programID;

    // std::unordered_map<int, OpenGLTexture2D*> textures;

    // std::unique_ptr<OpenGLUniformBuffer> m_mvp;

    bool m_isLink = false;
    std::unordered_map<uint32_t, std::unique_ptr<OpenGLUniformBuffer>> m_uniformDataBlocks;
};

}  // namespace Marbas

#endif
