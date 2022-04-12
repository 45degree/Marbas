#ifndef MARBAS_RHI_INTERFACE_VERTEX_BUFFER_H
#define MARBAS_RHI_INTERFACE_VERTEX_BUFFER_H

#include "Common.hpp"
#include <cstdint>
#include <glm/glm.hpp>
#include <numeric>

namespace Marbas {

enum class ElementType {
    BYTE,
    UNSIGNED_BYTE,
    SHORT,
    UNSIGNED_SHORT,
    INT,
    UNSIGNED_INT,
    FLOAT,
    HALF_FLOAT,
    DOUBLE
};

struct ElementLayout {
    int index;        /// Specifies the index of the generic vertex attribute
    ElementType mateType;
    size_t typeBytes;
    size_t count;     // Specifies the number of components per generic vertex attribute
    bool normalized;  // Specifies whether fixed-point data values should be normalized
    size_t stride;    // Specifies the byte offset between consecutive generic vertex attributes.
    size_t offset;    // Specifies the offset of the first component in the array.
};

/**
 * @brief A vertex buffer object (VBO) is an feature that provides methods for uploading
 *        vertex data (position, normal vector, color, etc.) to the video device for
 *        non-immediate-mode rendering.
 *
 * @see   https://en.wikipedia.org/wiki/Vertex_buffer_object
 *
 * @note  In the application, all vertex data is composed of floats, So, the vertex buffer data
 *        should be a float array
 *
 */
class VertexBuffer {
public:
    explicit VertexBuffer(size_t size) : m_size(size) {}
    virtual ~VertexBuffer() = default;
public:

    /**
     * @brief set the layout for each vertex buffer's element
     */
    void SetLayout(const Vector<ElementLayout>& layout);

    virtual void SetData(const void* data, size_t size, size_t offset) const = 0;
    virtual void Bind() const = 0;
    virtual void UnBind() const = 0;

    [[nodiscard]] size_t GetSize() const {
        return m_size;
    }

    [[nodiscard]] const Vector<ElementLayout>& Getlayout() const {
        return m_layout;
    }

protected:
    Vector<ElementLayout> m_layout;
    size_t m_size;
};


}  // namespace Marbas

#endif
