#pragma once

#include "RHI/Interface/RenderPass.hpp"
#include "RHI/OpenGL/OpenGLFrameBuffer.hpp"

namespace Marbas {

class OpenGLRenderPass final : public RenderPass {
 public:
  explicit OpenGLRenderPass(const RenderPassCreateInfo& createInfo) : RenderPass(createInfo) {}

  void
  Bind();
};

}  // namespace Marbas
