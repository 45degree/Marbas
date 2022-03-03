#include "Common.h"
#include "Renderer/OpenGL/OpenGLIndexBuffer.h"

namespace Marbas {

OpenGLIndexBuffer::OpenGLIndexBuffer(size_t size) {
    LOG(INFO) << "create opengl index buffer";

    glCreateBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::OpenGLIndexBuffer(const Vector<int>& data) {
    LOG(INFO) << "create opengl index buffer";

    auto size = static_cast<GLsizeiptr>(data.size() * sizeof(int));
    glCreateBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data.data(), GL_STATIC_DRAW);

    indexCount = size;
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
    LOG(INFO) << "delete opengl index buffer";
    glDeleteBuffers(1, &EBO);
}

void OpenGLIndexBuffer::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

void OpenGLIndexBuffer::UnBind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLIndexBuffer::SetData(const Vector<int> &data) {
    LOG(INFO) << "set data for opengl index buffer";

    auto size = static_cast<GLsizeiptr>(data.size() * sizeof(int));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data.data());

    indexCount = size;
}

}  // namespace Marbas
