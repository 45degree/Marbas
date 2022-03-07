#ifndef MARBARS_RENDERER_OPENGL_DRAWCALL_H
#define MARBARS_RENDERER_OPENGL_DRAWCALL_H

#include "Renderer/Interface/DrawCall.h"
#include "Renderer/Interface/IndexBuffer.h"
#include "Renderer/OpenGL/OpenGLTexture.h"

namespace Marbas {

class OpenGLDrawCall : public DrawCall{
public:
    OpenGLDrawCall();
    ~OpenGLDrawCall() override;

public:

    void AddVertices(const VertexBuffer* vertexBuffer, const VertexArray* verticesArray) override;

    void AddIndeices(const IndexBuffer* indices) override;

    void AddShader(const Shader* shader) override;

    void Link() override;

    void Draw() override;

    void Use() override;

    void AddTexture(Texture2D* texture, int uniformBind) override;

    void setBool(const String& name, bool value) const override;
    void setInt(const String& name, int value) const override;
    void setFloat(const String& name, float value) const override;
    void setVec2(const String& name, const glm::vec2 &value) const override;
    void setVec2(const String& name, float x, float y) const override;
    void setVec3(const String& name, const glm::vec3 &value) const override;
    void setVec3(const String& name, float x, float y, float z) const override;
    void setVec4(const String& name, const glm::vec4 &value) const override;
    void setVec4(const String& name, float x, float y, float z, float w) const override;
    void setMat2(const String& name, const glm::mat2 &mat) const override;
    void setMat3(const String& name, const glm::mat3 &mat) const override;
    void setMat4(const String& name, const glm::mat4 &mat) const override;

private:
    const VertexArray* verticesArray;
    const IndexBuffer* indexBuffer;
    GLuint programID;

    std::unordered_map<int, OpenGLTexture2D*> textures;
};

}  // namespace Marbas

#endif
