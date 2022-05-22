#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <numeric>

#include "Common/Common.hpp"
#include "RHI/Interface/IBuffer.hpp"

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
  int index = 0;  /// Specifies the index of the generic vertex attribute
  ElementType mateType = ElementType::FLOAT;
  size_t typeBytes = sizeof(float);
  size_t count = 0;         // Specifies the number of components per generic vertex attribute
  bool normalized = false;  // Specifies whether fixed-point data values should be normalized
  size_t stride = 0;  // Specifies the byte offset between consecutive generic vertex attributes.
  size_t offset = 0;  // Specifies the offset of the first component in the array.

  static void
  CalculateLayout(Vector<ElementLayout>& layout);
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
class VertexBuffer : public IBuffer {
 public:
  explicit VertexBuffer(size_t size) : m_size(size), m_stride(0) {}
  virtual ~VertexBuffer() = default;

 public:
  /**
   * @brief set the layout for each vertex buffer's element
   */
  void
  SetLayout(const Vector<ElementLayout>& layout);

  virtual void
  Bind() const = 0;

  virtual void
  UnBind() const = 0;

  [[nodiscard]] size_t
  GetSize() const {
    return m_size;
  }

  [[nodiscard]] const Vector<ElementLayout>&
  Getlayout() const {
    return m_layout;
  }

 protected:
  Vector<ElementLayout> m_layout;
  size_t m_size;
  size_t m_stride;
};

}  // namespace Marbas
