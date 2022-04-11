#ifndef MARBAS_RHI_INTERFACE_DRAWUNIT_H
#define MARBAS_RHI_INTERFACE_DRAWUNIT_H

#include "RHI/Interface/Shader.hpp"
#include "RHI/Interface/VertexBuffer.hpp"
#include "RHI/Interface/VertexArray.hpp"
#include "RHI/Interface/IndexBuffer.hpp"
#include "RHI/Interface/Texture.hpp"

namespace Marbas {

struct DrawUnit  {
    VertexBuffer* m_vertexBuffer = nullptr;
    IndexBuffer* m_indicesBuffer = nullptr;
    Vector<Texture2D*> textures;
};

class DrawBatch {
public:
    DrawBatch() = default;
    ~DrawBatch() = default;

public:
    void AddTexture(Texture2D* texture) {
        m_textures.push_back(texture);
    }

private:
    std::unique_ptr<VertexBuffer> m_vertexBuffer = nullptr;
    std::unique_ptr<IndexBuffer> m_indicesBuffer = nullptr;
    Vector<Texture2D*> m_textures;
    Shader* m_shader;
};

}  // namespace Marbas

#endif
