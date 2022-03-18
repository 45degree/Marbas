#ifndef MARBAS_RENDERER_INTERFACE_DRAWUNIT_H
#define MARBAS_RENDERER_INTERFACE_DRAWUNIT_H

#include "Renderer/Interface/Shader.h"
#include "Renderer/Interface/VertexBuffer.h"
#include "Renderer/Interface/VertexArray.h"
#include "Renderer/Interface/IndexBuffer.h"
#include "Renderer/Interface/Texture.h"

namespace Marbas {

struct DrawUnit  {
    VertexBuffer* m_vertexBuffer;
    IndexBuffer* m_indicesBuffer;
    Shader* m_shader;
    Vector<Texture2D*> textures;
};

}  // namespace Marbas

#endif
