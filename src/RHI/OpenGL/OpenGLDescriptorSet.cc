#include "RHI/OpenGL/OpenGLDescriptorSet.hpp"

#include <glog/logging.h>

#include "Common/Common.hpp"

namespace Marbas {

OpenGLDescriptorSet::OpenGLDescriptorSet(const DescriptorSetInfo& createInfo)
    : DescriptorSet(createInfo) {
  for (const auto& item : createInfo) {
    if (item.isBuffer) {
      m_bufferDescripor.insert({item.bindingPoint, nullptr});
    } else {
      m_imageDescriptor.insert({item.bindingPoint, nullptr});
    }
  }
}

void
OpenGLDescriptorSet::BindBuffer(uint16_t bindingPoint,
                                const std::shared_ptr<IBufferDescriptor>& descriptor) {
  // check whether the binding point is valid
  if (std::none_of(m_createInfo.cbegin(), m_createInfo.cend(), [&](const DescriptorInfo& item) {
        return item.isBuffer && item.type == descriptor->GetDescriptorType() &&
               bindingPoint == item.bindingPoint;
      })) {
    LOG(ERROR) << FORMAT("don't have this binding point {} for buffer descriptor", bindingPoint);
    return;
  }

  const auto openglDescriptor = std::dynamic_pointer_cast<IOpenGLBufferDescriptor>(descriptor);
  if (openglDescriptor != nullptr) {
    m_bufferDescripor[bindingPoint] = openglDescriptor;
  }
  LOG_IF(ERROR, openglDescriptor == nullptr) << FORMAT(
      "can't bind descriptor to {}, because it's not a opengl buffer descriptor", bindingPoint);
}

void
OpenGLDescriptorSet::BindImage(uint16_t bindingPoint,
                               const std::shared_ptr<IImageDescriptor>& descriptor) {
  // check
  if (std::none_of(m_createInfo.cbegin(), m_createInfo.cend(), [&](const DescriptorInfo& item) {
        return !item.isBuffer && bindingPoint == item.bindingPoint;
      })) {
    LOG(ERROR) << FORMAT("don't have this binding point {} for buffer descriptor", bindingPoint);
    return;
  }

  const auto openglDescriptor = std::dynamic_pointer_cast<IOpenGLImageDescriptor>(descriptor);
  if (openglDescriptor != nullptr) {
    m_imageDescriptor[bindingPoint] = openglDescriptor;
  }
  LOG_IF(ERROR, openglDescriptor == nullptr) << FORMAT(
      "can't bind descriptor to {}, because it's not a opengl buffer descriptor", bindingPoint);
}

void
OpenGLDescriptorSet::Bind() const {
  for (auto& [bindingPoint, bufferDescriptor] : m_bufferDescripor) {
    bufferDescriptor->Bind(bindingPoint);
  }

  for (auto& [bindingPoint, imageDescriptor] : m_imageDescriptor) {
    if (imageDescriptor != nullptr) {
      imageDescriptor->Bind(bindingPoint);
    }
  }
}

OpenGLDynamicDescriptorSet::OpenGLDynamicDescriptorSet(const Vector<uint16_t>& bindingPoints)
    : DynamicDescriptorSet(bindingPoints) {
  for (auto&& bindingPoint : bindingPoints) {
    m_bufferDescriptor.insert({bindingPoint, nullptr});
  }
}

void
OpenGLDynamicDescriptorSet::BindDynamicBuffer(
    uint16_t bindingPoint, const std::shared_ptr<IDynamicBufferDescriptor>& dynamicBuffer) {
  DLOG_ASSERT(std::find(m_bindingPoints.cbegin(), m_bindingPoints.cend(), bindingPoint) !=
              m_bindingPoints.cend())
      << FORMAT("don't have this binding point {} for buffer descriptor", bindingPoint);

  const auto openglDynamicDescriptor =
      std::dynamic_pointer_cast<IOpenGLDynamicBufferDescriptor>(dynamicBuffer);

  DLOG_ASSERT(openglDynamicDescriptor != nullptr)
      << FORMAT("can't bind descriptor to {}, because it's not a opengl dynamic buffer descriptor",
                bindingPoint);
  m_bufferDescriptor[bindingPoint] = openglDynamicDescriptor;
}

void
OpenGLDynamicDescriptorSet::Bind(uint32_t offset, uint32_t size) const {
  for (const auto& [bindingPoint, bufferDescripor] : m_bufferDescriptor) {
    bufferDescripor->Bind(bindingPoint, offset, size);
  }
}

}  // namespace Marbas
