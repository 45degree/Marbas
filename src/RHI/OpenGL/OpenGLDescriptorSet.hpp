#pragma once

#include <unordered_map>

#include "RHI/Interface/DescriptorSet.hpp"
#include "RHI/OpenGL/IOpenGLDescriptor.hpp"

namespace Marbas {

class OpenGLDescriptorSet : public DescriptorSet {
 public:
  explicit OpenGLDescriptorSet(const DescriptorSetInfo& createInfo);
  virtual ~OpenGLDescriptorSet() = default;

 public:
  void
  BindBuffer(uint16_t bindingPoint, const std::shared_ptr<IBufferDescriptor>& descriptor) override;

  void
  BindImage(uint16_t bindingPoint, const std::shared_ptr<IImageDescriptor>& descriptor) override;

  void
  Bind() const;

 private:
  std::unordered_map<uint16_t, std::shared_ptr<IOpenGLBufferDescriptor>> m_bufferDescripor;
  std::unordered_map<uint16_t, std::shared_ptr<IOpenGLImageDescriptor>> m_imageDescriptor;
};

class OpenGLDynamicDescriptorSet final : public DynamicDescriptorSet {
 public:
  explicit OpenGLDynamicDescriptorSet(const Vector<uint16_t>& bindingPoints);
  virtual ~OpenGLDynamicDescriptorSet() = default;

 public:
  void
  BindDynamicBuffer(uint16_t bindingPoint,
                    const std::shared_ptr<IDynamicBufferDescriptor>& dynamicBuffer) override;

  void
  Bind(uint32_t offset, uint32_t size) const;

 private:
  std::unordered_map<uint16_t, std::shared_ptr<IOpenGLDynamicBufferDescriptor>> m_bufferDescriptor;
};

}  // namespace Marbas
