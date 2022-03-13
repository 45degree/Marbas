#include "Renderer/OpenGL/OpenGLDrawCall.h"
#include "Renderer/OpenGL/OpenGLShader.h"
#include "Renderer/OpenGL/OpenGLTexture.h"

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

    for(auto& [bind, texture] : textures) {
        texture->Bind(bind);
    }

    verticesArray->Bind();
    indexBuffer->Bind();

    auto count = static_cast<GLsizei>(indexBuffer->GetIndexCount());
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void OpenGLDrawCall::AddTexture(Texture2D* texture, int uniformBind) {
    Use();

    auto openglTexture = dynamic_cast<OpenGLTexture2D*>(texture);
    if(openglTexture == nullptr) {
        LOG(ERROR) << "this texture is not a opengl 2D texture";
        return;
    }

    if(textures.find(uniformBind) != textures.end()) {
        LOG(ERROR) << FORMAT("failed to add this texture for uniform {}", uniformBind);
        return;
    }
    textures[uniformBind] = openglTexture;
}


void OpenGLDrawCall::setBool(const String& name, bool value) const {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), static_cast<int>(value));
}

void OpenGLDrawCall::setInt(const String& name, int value) const {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void OpenGLDrawCall::setFloat(const String& name, float value) const {
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void OpenGLDrawCall::setVec2(const String& name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}

void OpenGLDrawCall::setVec2(const String& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y); 
}

void OpenGLDrawCall::setVec3(const String& name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]); 
}

void OpenGLDrawCall::setVec3(const String& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}

void OpenGLDrawCall::setVec4(const String& name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}

void OpenGLDrawCall::setVec4(const String& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
}

void OpenGLDrawCall::setMat2(const String& name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void OpenGLDrawCall::setMat3(const String& name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void OpenGLDrawCall::setMat4(const String& name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

}  // namespace Marbas
