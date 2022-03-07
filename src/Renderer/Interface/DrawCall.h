#ifndef MARBARS_RENDER_INTERFACE_DRAWCALL_H
#define MARBARS_RENDER_INTERFACE_DRAWCALL_H

#include "Renderer/Interface/Shader.h"
#include "Renderer/Interface/VertexBuffer.h"
#include "Renderer/Interface/VertexArray.h"
#include "Renderer/Interface/IndexBuffer.h"
#include "Renderer/Interface/Texture.h"

namespace Marbas {

class DrawCall {
public:
    DrawCall() = default;
    virtual ~DrawCall() = default;

public:

    virtual void AddVertices(const VertexBuffer* vertexBuffer,
                             const VertexArray* verticesArray) = 0;

    virtual void AddIndeices(const IndexBuffer* indices) = 0;

    virtual void AddShader(const Shader* shader) = 0;

    virtual void Link() = 0;

    virtual void Draw() = 0;

    virtual void Use() = 0;

    virtual void AddTexture(Texture2D* texture, int uniformBind) = 0;

    // ------------------------------------------------------------------------
    virtual void setBool(const String& name, bool value) const = 0;
    virtual void setInt(const String& name, int value) const = 0;
    virtual void setFloat(const String& name, float value) const = 0;
    virtual void setVec2(const String& name, const glm::vec2 &value) const = 0;
    virtual void setVec2(const String& name, float x, float y) const = 0;
    virtual void setVec3(const String& name, const glm::vec3 &value) const = 0;
    virtual void setVec3(const String& name, float x, float y, float z) const = 0;
    virtual void setVec4(const String& name, const glm::vec4 &value) const = 0;
    virtual void setVec4(const String& name, float x, float y, float z, float w) const = 0;
    virtual void setMat2(const String& name, const glm::mat2 &mat) const = 0;
    virtual void setMat3(const String& name, const glm::mat3 &mat) const = 0;
    virtual void setMat4(const String& name, const glm::mat4 &mat) const = 0;
};

}  // namespace Marbas

#endif
