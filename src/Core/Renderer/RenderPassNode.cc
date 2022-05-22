#include "Core/Renderer/RenderPassNode.hpp"

namespace Marbas {

RenderPassNode::RenderPassNode(const RenderPassNodeCreateInfo& createInfo)
    : m_passName(createInfo.passName),
      m_rhiFactory(createInfo.rhiFactory),
      m_width(createInfo.width),
      m_height(createInfo.height) {
  for (const auto& input : createInfo.inputResource) {
    m_inputTarget.insert({input, nullptr});
  }

  for (const auto& output : createInfo.outputResource) {
    m_outputTarget.insert({output, nullptr});
  }
}

}  // namespace Marbas
