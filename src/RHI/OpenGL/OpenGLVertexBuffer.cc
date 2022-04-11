#include "RHI/OpenGL/OpenGLVertexBuffer.hpp"

#include <glog/logging.h>

namespace Marbas {

OpenGLVertexBuffer::OpenGLVertexBuffer(std::size_t size) : VBO(0) {
    LOG(INFO) << "create an opengl vertex buffer";
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_STATIC_DRAW);
    LOG(INFO) << "opengl vertex buffer create finish";
}

OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, size_t size) : VBO(0) {
    LOG(INFO) << "create an opengl vertex buffer";

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);

    LOG(INFO) << "opengl vertex buffer create finish";
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    LOG(INFO) << "delete vertex buffer";
    glDeleteBuffers(1, &VBO);
}

void OpenGLVertexBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    auto error = glGetError();
    LOG_IF(ERROR, error) << FORMAT("can't bind vertex buffer: {}, error code is {}", VBO, error);
}

void OpenGLVertexBuffer::UnBind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    auto error = glGetError();
    LOG_IF(ERROR, error) << FORMAT("can't unbind vertex buffer {}, error code is {}", VBO, error);
}

void OpenGLVertexBuffer::SetData(const void* data, size_t size) const {
    LOG(INFO) << "set vertex buffer data";

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), data);
}

}  // namespace Marbas
