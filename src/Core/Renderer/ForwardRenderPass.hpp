#pragma once

#include <concepts>

#include "Core/Renderer/RenderPassNode.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

struct ForwardRenderPassNodeCreateInfo : public RenderPassNodeCreateInfo {
  String inputPassNode;
};

class ForwardRenderPass : public RenderPassNode {
 public:
  explicit ForwardRenderPass(const ForwardRenderPassNodeCreateInfo& createInfo)
      : RenderPassNode(createInfo), m_inputPassNode(createInfo.inputPassNode) {}
  virtual ~ForwardRenderPass() = default;

 public:
  void
  Initialize() override {
    CreateRenderPass();
    CreateShader();
    CreateDescriptorSetLayout();
    CreatePipeline();
  }

  void
  SetFrameBuffer(const std::shared_ptr<FrameBuffer>& frameBuffer) {
    m_framebuffer = frameBuffer;
  }

  String
  GetInputTargetName() const {
    return m_inputPassNode;
  }

 protected:
  String m_inputPassNode;
};

}  // namespace Marbas
