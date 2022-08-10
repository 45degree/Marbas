#include "Core/Renderer/RenderTargetNode.hpp"

namespace Marbas {

RenderTargetNode::RenderTargetNode(const RenderTargetNodeCreateInfo& createInfo)
    : m_targetName(createInfo.targetName),
      m_gbufferType(createInfo.buffersType),
      m_rhiFactory(createInfo.rhiFactory),
      m_width(createInfo.width),
      m_height(createInfo.height) {
  // create gbuffer
  m_gbuffer = std::make_shared<GBuffer>(m_width, m_height);
  for (const auto& [bufferType, level] : m_gbufferType) {
    m_gbuffer->AddTexture(bufferType, level, m_rhiFactory);
  }
}

}  // namespace Marbas
