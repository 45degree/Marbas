#ifndef MARBAS_RHI_INTERFACE_DRAWUNIT_H
#define MARBAS_RHI_INTERFACE_DRAWUNIT_H

#include "RHI/Interface/Shader.hpp"
#include "RHI/Interface/VertexBuffer.hpp"
#include "RHI/Interface/VertexArray.hpp"
#include "RHI/Interface/IndexBuffer.hpp"
#include "RHI/Interface/Texture.hpp"

namespace Marbas {

struct DrawUnit  {
    VertexBuffer* m_vertexBuffer;
    IndexBuffer* m_indicesBuffer;
    Vector<Texture2D*> textures;
};

}  // namespace Marbas

#endif
