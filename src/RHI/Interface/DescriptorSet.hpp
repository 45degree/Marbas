#pragma once

#include <memory>

#include "Common/Common.hpp"
#include "RHI/Interface/IDescriptor.hpp"

namespace Marbas {

struct DescriptorInfo {
  bool isBuffer = false;
  BufferDescriptorType type;
  uint16_t bindingPoint = 0;
};

using DescriptorSetInfo = Vector<DescriptorInfo>;

class DescriptorSet {
 public:
  DescriptorSet(const DescriptorSetInfo& createInfo) : m_createInfo(createInfo){};
  virtual ~DescriptorSet() = default;

 public:
  virtual void
  BindBuffer(uint16_t bindingPoint, const std::shared_ptr<IBufferDescriptor>& descriptor) = 0;

  virtual void
  BindImage(uint16_t bindingPoint, const std::shared_ptr<IImageDescriptor>& descriptor) = 0;

 protected:
  DescriptorSetInfo m_createInfo;
};

class DynamicDescriptorSet {
 public:
  explicit DynamicDescriptorSet(const Vector<uint16_t>& bindingPoints)
      : m_bindingPoints(bindingPoints) {}

 public:
  virtual void
  BindDynamicBuffer(uint16_t bindingPoint,
                    const std::shared_ptr<IDynamicBufferDescriptor>& dynamicBuffer) = 0;

 protected:
  Vector<uint16_t> m_bindingPoints;
};

}  // namespace Marbas
