#include "PBRDirectLightPass.hpp"

namespace Marbas {

void
PBRDirectLightPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.ReadTexture(m_colorTexture);
  builder.ReadTexture(m_normalTexture);
  builder.ReadTexture(m_roughnessTexture);

  builder.SetFramebufferSize(m_width, m_height, 1);
}

void
PBRDirectLightPass::Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList) {}

}  // namespace Marbas
