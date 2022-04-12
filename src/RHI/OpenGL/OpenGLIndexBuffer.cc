#include "Common.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"
#include "RHI/OpenGL/OpenGLIndexBuffer.hpp"

#include <glog/logging.h>

namespace Marbas {

OpenGLIndexBuffer::OpenGLIndexBuffer(size_t count):
    IndexBuffer(count)
{
    LOG(INFO) << "create opengl index buffer";

    auto size = static_cast<GLsizeiptr>(count * sizeof(uint32_t));
    glCreateBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::OpenGLIndexBuffer(const Vector<uint32_t>& data):
    IndexBuffer(data.size())
{
    LOG(INFO) << "create opengl index buffer";

    auto size = static_cast<GLsizeiptr>(data.size() * sizeof(int));
    glCreateBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data.data(), GL_STATIC_DRAW);
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

void OpenGLIndexBuffer::SetData(const Vector<uint32_t> &data, size_t offsetCount) {
    LOG(INFO) << FORMAT("set data for opengl index buffer, the buffer count is {},"
                        " offset count is {}", m_indexCount, offsetCount);

    if(offsetCount + data.size() > m_indexCount) {
        LOG(ERROR) << FORMAT("can't set data for buffer, this buffer's max count is {}",
                             m_indexCount);
        return;
    }

    auto size = static_cast<GLsizeiptr>(data.size() * sizeof(uint32_t));
    auto offset = static_cast<GLsizeiptr>(offsetCount * sizeof(uint32_t));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(offset), size, data.data());
}

}  // namespace Marbas
