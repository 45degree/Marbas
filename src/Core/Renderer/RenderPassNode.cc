#include "Core/Renderer/RenderPassNode.hpp"

namespace Marbas {

RenderPassNode::RenderPassNode(const RenderPassNodeCreateInfo& createInfo)
    : m_passName(createInfo.passName),
      m_rhiFactory(createInfo.rhiFactory),
      m_width(createInfo.width),
      m_height(createInfo.height),
      m_resourceManager(createInfo.resourceManager) {
  auto bufferSize = sizeof(CameraUniformBlock);
  m_cameraUniformBuffer = m_rhiFactory->CreateUniformBuffer(bufferSize);

  // create command factory
  m_commandFactory = m_rhiFactory->CreateCommandFactory();
  m_commandBuffer = m_commandFactory->CreateCommandBuffer();

  // set binging point 0 as camera uniform buffer
  m_descriptorSetLayout = {DescriptorSetLayoutBinding{
      .isBuffer = true,
      .type = BufferDescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
  }};
}

void
RenderPassNode::AddDescriptorSetLayoutBinding(const DescriptorSetLayoutBinding& bindingInfo) {
  auto isDeuplication = std::any_of(m_descriptorSetLayout.cbegin(), m_descriptorSetLayout.cend(),
                                    [&](const DescriptorSetLayoutBinding& binding) -> bool {
                                      return binding.isBuffer == bindingInfo.isBuffer &&
                                             binding.bindingPoint == bindingInfo.bindingPoint;
                                    });

  if (isDeuplication) {
    LOG(ERROR) << FORMAT("{} existed a desciprot, we will not add this to the binding points",
                         bindingInfo.bindingPoint);
    return;
  }

  m_descriptorSetLayout.push_back(bindingInfo);
}

}  // namespace Marbas
