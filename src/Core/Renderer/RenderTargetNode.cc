#include "Core/Renderer/RenderTargetNode.hpp"

#include <glog/logging.h>

namespace Marbas {

RenderTargetNode::RenderTargetNode(const RenderTargetNodeCreateInfo& createInfo)
    : m_targetName(createInfo.targetName),
      m_gbufferType(createInfo.buffersType),
      m_rhiFactory(createInfo.rhiFactory),
      m_width(createInfo.width),
      m_height(createInfo.height) {
  // create gbuffer
  for (const auto& gBufferType : m_gbufferType) {
    const auto& type = gBufferType.type;
    const auto& levels = gBufferType.levels;
    const auto& layers = gBufferType.layers;
    auto gBuffer = std::make_shared<GBuffer>(m_width, m_height, m_rhiFactory, type, levels, layers);
    m_gbuffers.insert({type, gBuffer});
  }
}

std::shared_ptr<Texture>
RenderTargetNode::GetGBuffer(GBufferTexutreType type) const {
  DLOG_ASSERT(m_gbuffers.find(type) != m_gbuffers.end());
  return m_gbuffers.at(type)->GetTexture();
}

}  // namespace Marbas
