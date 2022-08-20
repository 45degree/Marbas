#include "Core/Renderer/DeferredRenderPass.hpp"

namespace Marbas {

DeferredRenderPass::DeferredRenderPass(const DeferredRenderPassCreateInfo& createInfo)
    : RenderPassNode(createInfo) {
  for (const auto& input : createInfo.inputResource) {
    m_inputTarget.insert({input, nullptr});
  }

  for (const auto& output : createInfo.outputResource) {
    m_outputTarget.insert({output, nullptr});
  }
}

}  // namespace Marbas
