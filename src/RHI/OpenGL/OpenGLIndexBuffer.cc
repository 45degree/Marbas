#include "Common.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"
#include "RHI/OpenGL/OpenGLIndexBuffer.hpp"

#include <glog/logging.h>

namespace Marbas {

OpenGLIndexBuffer::OpenGLIndexBuffer(size_t size) {
    LOG(INFO) << "create opengl index buffer";

    glCreateBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::OpenGLIndexBuffer(const Vector<uint32_t>& data) {
    LOG(INFO) << "create opengl index buffer";

    auto size = static_cast<GLsizeiptr>(data.size() * sizeof(int));
    glCreateBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data.data(), GL_STATIC_DRAW);

    indexCount = data.size();
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
    LOG(INFO) << "delete opengl index buffer";
    glDeleteBuffers(1, &EBO);
}

void OpenGLIndexBuffer::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    auto error = glGetError();
    LOG_IF(ERROR, error) << FORMAT("can't bind index buffer: {}", EBO);
}

void OpenGLIndexBuffer::UnBind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    auto error = glGetError();
    LOG_IF(ERROR, error) << FORMAT("can't unbind index buffer: {}", EBO);
}

void OpenGLIndexBuffer::SetData(const Vector<uint32_t> &data) {
    LOG(INFO) << "set data for opengl index buffer";

    auto size = static_cast<GLsizeiptr>(data.size() * sizeof(int));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data.data());

    indexCount = data.size();
}

}  // namespace Marbas
