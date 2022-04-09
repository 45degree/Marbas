#ifndef MARBAS_RHI_OPENGL_DRAWCOLLECTION_H
#define MARBAS_RHI_OPENGL_DRAWCOLLECTION_H

#include "RHI/Interface/DrawCollection.hpp"
#include "RHI/OpenGL/OpenGLVertexArray.hpp"

namespace Marbas {

class OpenGLDrawCollection : public DrawCollection{
public:
    OpenGLDrawCollection();
    ~OpenGLDrawCollection() override = default;

public:
    void Draw(Shader* Shader) const override;

private:
    std::unique_ptr<OpenGLVertexArray> m_vertexArray;
};

}  // namespace Marbas

#endif
