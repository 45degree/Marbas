#include "Renderer/OpenGL/OpenGLVertexBuffer.h"

namespace Marbas {

OpenGLVertexBuffer::OpenGLVertexBuffer(std::size_t size) {
    LOG(INFO) << "create an opengl vertex buffer";
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(const Vector<float>& data) {
    LOG(INFO) << "create an opengl vertex buffer";

    auto size = static_cast<GLsizeiptr>(data.size());
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, data.data(), GL_STATIC_DRAW);
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

    auto size = static_cast<GLsizeiptr>(data.size());
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data.data());
}

}  // namespace Marbas
