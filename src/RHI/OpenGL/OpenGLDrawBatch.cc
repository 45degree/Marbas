#include "RHI/OpenGL/OpenGLDrawBatch.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

constexpr GLenum GetOpenGLBlendFactor(BlendFactor type) {
    switch(type) {
    case BlendFactor::ZERO: return GL_ZERO;
    case BlendFactor::ONE: return GL_ONE;
    case BlendFactor::SRC_COLOR: return GL_SRC_COLOR;
    case BlendFactor::ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
    case BlendFactor::DST_COLOR: return GL_DST_COLOR;
    case BlendFactor::ONE_MINUS_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
    case BlendFactor::SRC_ALPHA: return GL_SRC_ALPHA;
    case BlendFactor::ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DST_ALPHA: return GL_DST_ALPHA;
    case BlendFactor::ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
    case BlendFactor::CONSTANT_COLOR: return GL_CONSTANT_COLOR;
    case BlendFactor::ONE_MINUS_CONSTANT_COLOR: return GL_ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::CONSTANT_ALPHA: return GL_CONSTANT_ALPHA;
    case BlendFactor::ONE_MINUS_CONSTANT_ALPHA: return GL_ONE_MINUS_CONSTANT_ALPHA;
    }
}

void OpenGLDrawBatch::Draw() {
    if(!IsComplete()) return;

    // auto [srcBlendFactor, dstBlendFactor] = m_material->GetBlendFactor();
    // glBlendFunc(GetOpenGLBlendFactor(srcBlendFactor), GetOpenGLBlendFactor(dstBlendFactor));

    // set textures
    auto allTextures = m_material->GetAllTextures();

    int i = 0;
    for(auto* texture : allTextures) {
        texture->Bind(i++);
    }

    // set data
    m_vertexArray->EnableVertexAttribArray(m_vertexBuffer.get());
    m_vertexArray->Bind();
    m_vertexBuffer->Bind();
    m_indexBuffer->Bind();

    // draw
    const auto* shader = m_material->GetShader();
    shader->Use();
    auto count = static_cast<GLsizei>(m_indexBuffer->GetIndexCount());
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

    // unbind all data
    m_vertexBuffer->UnBind();
    m_indexBuffer->UnBind();
    for(auto* texture : allTextures) {
        texture->UnBind();
    }
}

}  // namespace Marbas
