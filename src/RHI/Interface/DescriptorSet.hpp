#pragma once

#include <memory>

#include "Common/Common.hpp"
#include "RHI/Interface/IDescriptor.hpp"
#include "RHI/Interface/Texture.hpp"
#include "RHI/Interface/UniformBuffer.hpp"

namespace Marbas {

struct DescriptorSetLayoutBinding {
  bool isBuffer = false;
  BufferDescriptorType type;
  uint16_t bindingPoint = 0;
};

using DescriptorSetLayout = Vector<DescriptorSetLayoutBinding>;

class DescriptorSet {
 public:
  DescriptorSet(const DescriptorSetLayout& createInfo) : m_createInfo(createInfo){};
  virtual ~DescriptorSet() = default;

 public:
  virtual void
  BindBuffer(uint16_t bindingPoint, const std::shared_ptr<UniformBuffer>& uniformBuffer) = 0;

  virtual void
  BindImage(uint16_t bindingPoint, const std::shared_ptr<Texture>& texture) = 0;

  virtual void
  BindDynamicBuffer(uint16_t bindingPoint,
                    const std::shared_ptr<DynamicUniformBuffer>& uniformBuffer) = 0;

 protected:
  DescriptorSetLayout m_createInfo;
};

}  // namespace Marbas
