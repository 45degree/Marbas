#include "RHI/OpenGL/OpenGLVertexArray.h"

namespace Marbas {

OpenGLVertexArray::OpenGLVertexArray() {
    LOG(INFO) << "create vertex array";

    glGenVertexArrays(1, &VAO);
}

OpenGLVertexArray::~OpenGLVertexArray() {
    LOG(INFO) << "delete vertex array;";
    glDeleteVertexArrays(1, &VAO);
}

void OpenGLVertexArray::Bind() const {
    glBindVertexArray(VAO);

    auto error = glGetError();
    LOG_IF(ERROR, error) << FORMAT("can't bind vertex array {}, error code is {}", VAO, error);
}

void OpenGLVertexArray::UnBind() const {
    glBindVertexArray(0);

    auto error = glGetError();
    LOG_IF(ERROR, error) << FORMAT("can't unbind vertex array {}, error code is {}", VAO, error);
}

void OpenGLVertexArray::EnableVertexAttribArray(const VertexBuffer* vertexBuffer) const {
    Bind();
    vertexBuffer->Bind();

    const auto& layout = vertexBuffer->Getlayout();

    for(const auto& elementInfo : layout) {
        GLuint index = elementInfo.index;
        auto size = static_cast<GLint>(elementInfo.size);
        GLboolean isNormalized = elementInfo.normalized;
        auto stride = static_cast<GLsizei>(elementInfo.stride);
        auto offset = reinterpret_cast<const void*>(elementInfo.offset);

        glEnableVertexAttribArray(elementInfo.index);
        glVertexAttribPointer(index, size, GL_FLOAT, isNormalized, stride, offset);
    }
    //
    // vertexBuffer->UnBind();
    // UnBind();
}

}  // namespace Marbas
