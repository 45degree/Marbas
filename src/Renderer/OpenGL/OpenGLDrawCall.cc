#include "Renderer/OpenGL/OpenGLDrawCall.h"
#include "Renderer/OpenGL/OpenGLShader.h"

namespace Marbas {

OpenGLDrawCall::OpenGLDrawCall() {
    programID = glCreateProgram();
}

OpenGLDrawCall::~OpenGLDrawCall() {
    glDeleteProgram(programID);
}

void OpenGLDrawCall::AddVertices(const VertexBuffer* vertexBuffer,
                                 const VertexArray* verticesArray) {
    this->verticesArray = verticesArray;
    verticesArray->EnableVertexAttribArray(vertexBuffer);
}

void OpenGLDrawCall::AddIndeices(const IndexBuffer *indices) {
    this->indexBuffer = indices;
}

void OpenGLDrawCall::AddShader(const Shader* shader) {
    auto openglShader =  dynamic_cast<const OpenGLShader*>(shader);
    if(openglShader == nullptr) {
        LOG(ERROR) << "shader is not a opengl shader";
        throw std::runtime_error("shader can't dynamic cast to OpenglShader");
    }
    auto shaderId = openglShader->GetShaderID();
    glAttachShader(programID, shaderId);
}

void OpenGLDrawCall::Link() {
    glLinkProgram(programID);
    GLint success = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        LOG(INFO) << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
    }
}

void OpenGLDrawCall::Use() {
    glUseProgram(programID);
}

void OpenGLDrawCall::Draw() {
    verticesArray->Bind();
    indexBuffer->Bind();

    auto count = static_cast<GLsizei>(indexBuffer->GetIndexCount());
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

}  // namespace Marbas
