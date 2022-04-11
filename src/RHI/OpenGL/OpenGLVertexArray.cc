#include "RHI/OpenGL/OpenGLVertexArray.hpp"

#include <glog/logging.h>

namespace Marbas {

static GLenum ConvertToOpenGLType(ElementType type) {
    switch(type){
    case ElementType::BYTE: return GL_BYTE;
    case ElementType::UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
    case ElementType::SHORT: return GL_SHORT;
    case ElementType::UNSIGNED_SHORT: return GL_UNSIGNED_SHORT;
    case ElementType::INT: return GL_INT;
    case ElementType::UNSIGNED_INT: return GL_UNSIGNED_INT;
    case ElementType::HALF_FLOAT: return GL_HALF_FLOAT;
    case ElementType::FLOAT: return GL_FLOAT;
    case ElementType::DOUBLE: return GL_DOUBLE;
    }
}

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
        auto size = static_cast<GLint>(elementInfo.count);
        GLboolean isNormalized = elementInfo.normalized;
        auto stride = static_cast<GLsizei>(elementInfo.stride);
        auto offset = reinterpret_cast<const void*>(elementInfo.offset);

        glEnableVertexAttribArray(elementInfo.index);
        glVertexAttribPointer(index, size, ConvertToOpenGLType(elementInfo.mateType),
                              isNormalized, stride, offset);
    }
}

}  // namespace Marbas
