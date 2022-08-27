#include "RHI/OpenGL/OpenGLDescriptorSet.hpp"

#include <glog/logging.h>

#include "Common/Common.hpp"

namespace Marbas {

OpenGLDescriptorSet::OpenGLDescriptorSet(const DescriptorSetLayout& createInfo)
    : DescriptorSet(createInfo) {
  for (const auto& item : createInfo) {
    if (item.isBuffer && item.type == BufferDescriptorType::UNIFORM_BUFFER) {
      m_bufferDescripor.insert({item.bindingPoint, nullptr});
    } else if (item.isBuffer && item.type == BufferDescriptorType::DYNAMIC_UNIFORM_BUFFER) {
      m_dynamicBuffer.insert({item.bindingPoint, nullptr});
    } else {
      m_imageDescriptor.insert({item.bindingPoint, nullptr});
    }
  }
}

void
OpenGLDescriptorSet::BindBuffer(uint16_t bindingPoint,
                                const std::shared_ptr<UniformBuffer>& descriptor) {
  // check whether the binding point is valid
  if (std::none_of(m_createInfo.cbegin(), m_createInfo.cend(),
                   [&](const DescriptorSetLayoutBinding& item) {
                     return item.isBuffer && item.type == descriptor->GetBufferDescriptorType() &&
                            bindingPoint == item.bindingPoint;
                   })) {
    LOG(ERROR) << FORMAT("don't have this binding point {} for buffer descriptor", bindingPoint);
    return;
  }

  const auto openglDescriptor = std::dynamic_pointer_cast<OpenGLUniformBuffer>(descriptor);
  if (openglDescriptor != nullptr) {
    m_bufferDescripor[bindingPoint] = openglDescriptor;
  }
  LOG_IF(ERROR, openglDescriptor == nullptr) << FORMAT(
      "can't bind descriptor to {}, because it's not a opengl buffer descriptor", bindingPoint);
}

void
OpenGLDescriptorSet::BindImage(uint16_t bindingPoint, const std::shared_ptr<Texture>& descriptor) {
  // check
  if (std::none_of(m_createInfo.cbegin(), m_createInfo.cend(),
                   [&](const DescriptorSetLayoutBinding& item) {
                     return !item.isBuffer && bindingPoint == item.bindingPoint;
                   })) {
    LOG(ERROR) << FORMAT("don't have this binding point {} for buffer descriptor", bindingPoint);
    return;
  }

  const auto openglTextureCubeMap = std::dynamic_pointer_cast<OpenGLTextureCubeMap>(descriptor);
  const auto openglTexture2D = std::dynamic_pointer_cast<OpenGLTexture2D>(descriptor);
  if (openglTexture2D != nullptr) {
    m_imageDescriptor[bindingPoint] = openglTexture2D;
  } else if (openglTextureCubeMap != nullptr) {
    m_imageDescriptor[bindingPoint] = openglTextureCubeMap;
  } else {
    LOG(ERROR) << FORMAT("can't bind descriptor to {}, because it's not a opengl buffer descriptor",
                         bindingPoint);
  }
}

void
OpenGLDescriptorSet::BindDynamicBuffer(uint16_t bindingPoint,
                                       const std::shared_ptr<DynamicUniformBuffer>& dynamicBuffer) {
  // check whether the binding point is valid
  if (std::none_of(
          m_createInfo.cbegin(), m_createInfo.cend(), [&](const DescriptorSetLayoutBinding& item) {
            return item.isBuffer && item.type == dynamicBuffer->GetBufferDescriptorType() &&
                   bindingPoint == item.bindingPoint;
          })) {
    LOG(ERROR) << FORMAT("don't have this binding point {} for buffer descriptor", bindingPoint);
    return;
  }

  const auto openglDynamicBuffer =
      std::dynamic_pointer_cast<OpenGLDynamicUniformBuffer>(dynamicBuffer);
  if (openglDynamicBuffer != nullptr) {
    m_dynamicBuffer[bindingPoint] = openglDynamicBuffer;
  }
  LOG_IF(ERROR, openglDynamicBuffer == nullptr) << FORMAT(
      "can't bind descriptor to {}, because it's not a opengl buffer descriptor", bindingPoint);
}

void
OpenGLDescriptorSet::Bind(const Vector<DynamicBufferPiece>& bufferPiece) const {
  for (auto& [bindingPoint, bufferDescriptor] : m_bufferDescripor) {
    bufferDescriptor->Bind(bindingPoint);
  }

  for (auto& [bindingPoint, imageDescriptor] : m_imageDescriptor) {
    if (imageDescriptor != nullptr) {
      imageDescriptor->Bind(bindingPoint);
    }
  }

  for (int i = 0, j = 0; i < m_createInfo.size(); i++) {
    if (m_createInfo[i].type != BufferDescriptorType::DYNAMIC_UNIFORM_BUFFER) continue;
    auto bindingPoint = m_createInfo[i].bindingPoint;
    const auto& buffer = m_dynamicBuffer.at(bindingPoint);
    buffer->Bind(bindingPoint, bufferPiece[j].offset, bufferPiece[j].size);

    j++;
  }
}

}  // namespace Marbas
