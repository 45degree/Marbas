#include "Core/Renderer/RenderPassNode.hpp"

namespace Marbas {

RenderPassNode::RenderPassNode(const RenderPassNodeCreateInfo& createInfo)
    : m_passName(createInfo.passName),
      m_rhiFactory(createInfo.rhiFactory),
      m_width(createInfo.width),
      m_height(createInfo.height),
      m_resourceManager(createInfo.resourceManager) {}

}  // namespace Marbas
