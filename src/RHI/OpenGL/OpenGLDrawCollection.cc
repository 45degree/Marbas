#include "RHI/OpenGL/OpenGLDrawCollection.hpp"

namespace Marbas {

OpenGLDrawCollection::OpenGLDrawCollection() : DrawCollection() {
    m_vertexArray = std::make_unique<OpenGLVertexArray>();
}

void OpenGLDrawCollection::Draw(Shader* shader) const {
    for(auto* drawUtil : m_drawUnits) {

        // DrawUnit* drawUtil = *(m_drawUnits.begin());
        int i = 0;
        for(auto* texture : drawUtil->textures) {
            texture->Bind(i++);
        }
        auto textureSize = drawUtil->textures.size();
        shader->AddUniformDataBlock(1, &textureSize, sizeof(decltype(textureSize)));

        m_vertexArray->EnableVertexAttribArray(drawUtil->m_vertexBuffer);
        m_vertexArray->Bind();
        drawUtil->m_vertexBuffer->Bind();
        drawUtil->m_indicesBuffer->Bind();
        shader->Use();

        auto count = static_cast<GLsizei>(drawUtil->m_indicesBuffer->GetIndexCount());
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

        drawUtil->m_vertexBuffer->UnBind();
        drawUtil->m_indicesBuffer->UnBind();

        for(auto* texture : drawUtil->textures) {
            texture->UnBind();
        }
    }
}

}  // namespace Marbas
