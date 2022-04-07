#ifndef MARBAS_RHI_INTERFACE_DRAWUNIT_H
#define MARBAS_RHI_INTERFACE_DRAWUNIT_H

#include "RHI/Interface/Shader.h"
#include "RHI/Interface/VertexBuffer.h"
#include "RHI/Interface/VertexArray.h"
#include "RHI/Interface/IndexBuffer.h"
#include "RHI/Interface/Texture.h"

namespace Marbas {

struct DrawUnit  {
    VertexBuffer* m_vertexBuffer;
    IndexBuffer* m_indicesBuffer;
    Vector<Texture2D*> textures;
};

}  // namespace Marbas

#endif
