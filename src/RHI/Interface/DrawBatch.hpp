#ifndef MARBAS_RHI_INTERFACE_DRAWUNIT_H
#define MARBAS_RHI_INTERFACE_DRAWUNIT_H

#include "RHI/Interface/IndexBuffer.hpp"
#include "RHI/Interface/Material.hpp"
#include "RHI/Interface/Shader.hpp"
#include "RHI/Interface/VertexArray.hpp"
#include "RHI/Interface/VertexBuffer.hpp"

namespace Marbas {

class DrawBatch {
 public:
  DrawBatch() = default;
  ~DrawBatch() = default;

 public:
  virtual void Draw() = 0;

  void SetVertexBuffer(std::unique_ptr<VertexBuffer>&& vertexBuffer) {
    m_vertexBuffer = std::move(vertexBuffer);
  }

  void SetIndexBuffer(std::unique_ptr<IndexBuffer>&& indexBuffer) {
    m_indexBuffer = std::move(indexBuffer);
  }

  void SetVertexArray(std::unique_ptr<VertexArray>&& vertexArray) {
    m_vertexArray = std::move(vertexArray);
  }

  void SetMaterial(MaterialBase* material) noexcept { m_material = material; }

  [[nodiscard]] MaterialBase* GetMaterial() const noexcept { return m_material; }

  void EnableBlend(bool isEnable) noexcept { m_enableBlend = isEnable; }

  [[nodiscard]] bool IsEnableBlend() const noexcept { return m_enableBlend; }

  void SetBlendFactor(const BlendFactorInfo& blendFactorInfo) {
    m_srcBlendFactor = blendFactorInfo.srcBlendType;
    m_dstBlendFactor = blendFactorInfo.dstBlendType;
  }

  [[nodiscard]] bool IsComplete() const noexcept {
    return !(m_vertexBuffer == nullptr || m_vertexArray == nullptr || m_indexBuffer == nullptr ||
             m_material == nullptr);
  }

 protected:
  std::unique_ptr<VertexBuffer> m_vertexBuffer = nullptr;
  std::unique_ptr<VertexArray> m_vertexArray = nullptr;
  std::unique_ptr<IndexBuffer> m_indexBuffer = nullptr;

  MaterialBase* m_material;

  bool m_enableBlend = false;
  BlendFactor m_srcBlendFactor = BlendFactor::SRC_ALPHA;
  BlendFactor m_dstBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
};

}  // namespace Marbas

#endif
