#include "Renderer/OpenGL/OpenGLUniformBuffer.h"

namespace Marbas {

OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t bindingPoint) :
    UniformBuffer(size, bindingPoint) {

    glCreateBuffers(1, &UBO);
    Bind();
    glBufferData(GL_UNIFORM_BUFFER, m_size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, UBO);
    UnBind();

    LOG(INFO) << "create uniform buffer";
}

OpenGLUniformBuffer::~OpenGLUniformBuffer() {
    glDeleteBuffers(1, &UBO);
    LOG(INFO) << "delete uniform buffer";
}

void OpenGLUniformBuffer::Bind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);

    auto error = glGetError();
    LOG_IF(ERROR, error) << FORMAT("can't bind uniform buffer, the error code is {}", error);
}

void OpenGLUniformBuffer::UnBind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    auto error = glGetError();
    LOG_IF(ERROR, error) << FORMAT("can't unbind uniform buffer, the error code is {}", error);
}

void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset) {
    Bind();

    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);

    auto error = glGetError();
    LOG_IF(ERROR, error) << 
        FORMAT("can't set data for uniform buffer, the error code is {}", error);

    UnBind();
}

}  // namespace Marbas
