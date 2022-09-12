#pragma once

#include <unordered_map>

#include "RHI/Interface/CommandBuffer.hpp"
#include "RHI/Interface/DescriptorSet.hpp"
#include "RHI/OpenGL/IOpenGLCommon.hpp"
#include "RHI/OpenGL/OpenGLTexture.hpp"
#include "RHI/OpenGL/OpenGLUniformBuffer.hpp"

namespace Marbas {

class OpenGLDescriptorSet final : public DescriptorSet {
 public:
  explicit OpenGLDescriptorSet(const DescriptorSetLayout& createInfo);
  virtual ~OpenGLDescriptorSet() = default;

 public:
  void
  BindBuffer(uint16_t bindingPoint, const std::shared_ptr<UniformBuffer>& descriptor) override;

  void
  BindImage(uint16_t bindingPoint, const std::shared_ptr<Texture>& descriptor) override;

  void
  BindDynamicBuffer(uint16_t bindingPoint,
                    const std::shared_ptr<DynamicUniformBuffer>& dynamicBuffer) override;

  void
  Bind(const Vector<DynamicBufferPiece>& bufferPiece) const;

 private:
  std::unordered_map<uint16_t, std::shared_ptr<OpenGLUniformBuffer>> m_bufferDescripor;
  std::unordered_map<uint16_t, std::shared_ptr<OpenGLTexture>> m_imageDescriptor;
  std::unordered_map<uint16_t, std::shared_ptr<OpenGLDynamicUniformBuffer>> m_dynamicBuffer;
};

}  // namespace Marbas
