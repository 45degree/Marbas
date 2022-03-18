#ifndef MARBAS_RENDERER_OPENGL_DRAWCOLLECTION_H
#define MARBAS_RENDERER_OPENGL_DRAWCOLLECTION_H

#include "Renderer/Interface/DrawCollection.h"
#include "Renderer/OpenGL/OpenGLVertexArray.h"

namespace Marbas {

class OpenGLDrawCollection : public DrawCollection{
public:
    OpenGLDrawCollection();
    ~OpenGLDrawCollection() override = default;

public:
    void Draw() override;

private:
    std::unique_ptr<OpenGLVertexArray> m_vertexArray;
};

}  // namespace Marbas

#endif
