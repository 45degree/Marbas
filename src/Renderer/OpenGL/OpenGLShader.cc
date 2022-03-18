#include "Renderer/OpenGL/OpenGLShader.h"
#include "Renderer/OpenGL/OpenGLShaderCode.h"
#include "Renderer/OpenGL/OpenGLTexture.h"

namespace Marbas {

OpenGLShader::OpenGLShader() {
    programID = glCreateProgram();
    // m_mvp = std::make_unique<OpenGLUniformBuffer>(3 * sizeof(glm::mat4));
    // m_mvp->SetBindingPoint(0);
}

OpenGLShader::~OpenGLShader() {
    glDeleteProgram(programID);
}

// void OpenGLShader::AddVertices(const VertexBuffer* vertexBuffer,
//                                  const VertexArray* verticesArray) {
//     this->verticesArray = verticesArray;
//     verticesArray->EnableVertexAttribArray(vertexBuffer);
// }

// void OpenGLShader::AddIndeices(const IndexBuffer *indices) {
//     this->indexBuffer = indices;
// }

void OpenGLShader::AddShaderCode(const ShaderCode* shaderCode) {
    auto openglShader =  dynamic_cast<const OpenGLShaderCode*>(shaderCode);
    if(openglShader == nullptr) {
        LOG(ERROR) << "shader is not a opengl shader";
        throw std::runtime_error("shader can't dynamic cast to OpenglShader");
    }
    auto shaderId = openglShader->GetShaderID();
    glAttachShader(programID, shaderId);
}


// void OpenGLShader::SetMvp(const glm::mat4& model, const glm::mat4& view, 
//                             const glm::mat4& projection) {
//
//     if(!m_isLink) return;
//
//     m_mvp->BindToBindPoint();
//     m_mvp->SetData(glm::value_ptr(model), sizeof(model), 0);
//     m_mvp->SetData(glm::value_ptr(view), sizeof(view), sizeof(glm::mat4));
//     m_mvp->SetData(glm::value_ptr(projection), sizeof(glm::mat4), sizeof(glm::mat4));
// }

void OpenGLShader::Link() {
    glLinkProgram(programID);
    GLint success = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        LOG(ERROR) << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
    }

    // GLuint blockIndex = glGetUniformBlockIndex(programID, "Matrices");
    //
    // auto error = glGetError();
    // LOG(INFO) << "error1:" << error;
    //
    // GLint blockSize;
    // glGetActiveUniformBlockiv(programID, blockIndex,GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
    //
    // error = glGetError();
    // LOG(INFO) << "error2:" << error;
    // LOG(INFO) << blockSize;

    m_isLink = true;
}

void OpenGLShader::AddUniformDataBlock(uint32_t bindingPoint, const void *data, uint32_t size) {
    if(m_uniformDataBlocks.find(bindingPoint) == m_uniformDataBlocks.end()) {
        m_uniformDataBlocks[bindingPoint] = std::make_unique<OpenGLUniformBuffer>(size, bindingPoint);
    }

    auto* uniformBlock = m_uniformDataBlocks.at(bindingPoint).get();
    uniformBlock->SetData(data, size, 0);
}

void OpenGLShader::Use() {
    glUseProgram(programID);
    for(auto& [bindingPoint, buffer] : m_uniformDataBlocks) {
        buffer->Bind();
    }
}

// void OpenGLShader::Draw() {
//     for(auto& [bind, texture] : textures) {
//         texture->Bind(bind);
//     }
//
//     verticesArray->Bind();
//     indexBuffer->Bind();
//     m_mvp->Bind();
//
//     auto count = static_cast<GLsizei>(indexBuffer->GetIndexCount());
//     glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
// }

// void OpenGLShader::AddTexture(Texture2D* texture, int uniformBind) {
//     Use();
//
//     auto openglTexture = dynamic_cast<OpenGLTexture2D*>(texture);
//     if(openglTexture == nullptr) {
//         LOG(ERROR) << "this texture is not a opengl 2D texture";
//         return;
//     }
//
//     if(textures.find(uniformBind) != textures.end()) {
//         LOG(ERROR) << FORMAT("failed to add this texture for uniform {}", uniformBind);
//         return;
//     }
//     textures[uniformBind] = openglTexture;
// }

}  // namespace Marbas
