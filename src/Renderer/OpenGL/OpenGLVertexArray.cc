#include "Renderer/OpenGL/OpenGLVertexArray.h"

namespace Marbas {

OpenGLVertexArray::OpenGLVertexArray() {
    LOG(INFO) << "create vertex array";

    glGenVertexArrays(1, &VAO);
}

OpenGLVertexArray::~OpenGLVertexArray() {
    LOG(INFO) << "delete vertex array;";
    glDeleteVertexArrays(1, &VAO);
}

void OpenGLVertexArray::Bind() {
    LOG(INFO) << "bind vertex array;";

    glBindVertexArray(VAO);
}

void OpenGLVertexArray::UnBind() {
    LOG(INFO) << "unbind vertex array;";
    glBindVertexArray(0);
}

void OpenGLVertexArray::EnableVertexAttribArray(const VertexBuffer& vertexBuffer) {
    glBindVertexArray(VAO);
    vertexBuffer.Bind();

    const auto& layout = vertexBuffer.Getlayout();

    for(const auto& elementInfo : layout) {
        GLuint index = elementInfo.index;
        auto size = static_cast<GLint>(elementInfo.size);
        GLboolean isNormalized = elementInfo.normalized;
        auto stride = static_cast<GLsizei>(elementInfo.stride);
        auto offset = reinterpret_cast<const void*>(elementInfo.offset);

        glEnableVertexAttribArray(elementInfo.index);
        glVertexAttribPointer(index, size, GL_FLOAT, isNormalized, stride, offset);
    }
}

}  // namespace Marbas
