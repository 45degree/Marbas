#include "Renderer/OpenGL/OpenGLVertexBuffer.h"

namespace Marbas {

OpenGLVertexBuffer::OpenGLVertexBuffer(std::size_t size) : VBO(0) {
    LOG(INFO) << "create an opengl vertex buffer";
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_STATIC_DRAW);
    LOG(INFO) << "opengl vertex buffer create finish";
}

OpenGLVertexBuffer::OpenGLVertexBuffer(const Vector<float>& data) : VBO(0) {
    LOG(INFO) << "create an opengl vertex buffer";

    auto size = static_cast<GLsizeiptr>(data.size()) * sizeof(float);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, data.data(), GL_STATIC_DRAW);

    LOG(INFO) << "opengl vertex buffer create finish";
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    LOG(INFO) << "delete vertex buffer";
    glDeleteBuffers(1, &VBO);
}

void OpenGLVertexBuffer::Bind() const {
    LOG(INFO) << "bind the opengl vertex buffer";
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void OpenGLVertexBuffer::UnBind() const {
    LOG(INFO) << "unbind the opengl vertex buffer";
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::SetData(const Vector<float>& data) const {
    LOG(INFO) << "set vertex buffer data";

    auto size = static_cast<GLsizeiptr>(data.size()) * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data.data());
}

}  // namespace Marbas
