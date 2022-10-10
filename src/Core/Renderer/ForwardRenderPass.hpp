#pragma once

#include <concepts>

#include "Core/Renderer/RenderPassNode.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

struct ForwardRenderPassNodeCreateInfo : public RenderPassNodeCreateInfo {
  String inputPassNode;
  Vector<String> inputResource;
};

class ForwardRenderPass : public RenderPassNode {
 public:
  explicit ForwardRenderPass(const ForwardRenderPassNodeCreateInfo& createInfo)
      : RenderPassNode(createInfo), m_inputPassNode(createInfo.inputPassNode) {
    for (const auto& input : createInfo.inputResource) {
      m_inputTarget.insert({input, nullptr});
    }
  }
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

  /**
   * render graph while use these methodes to set graph dependence
   */

  /**
   * @berif get all input render target name
   */
  Vector<String>
  GetAllInputTargetName() const {
    Vector<String> res;
    for (const auto& [name, resource] : m_inputTarget) {
      res.push_back(name);
    }
    return res;
  }

  /**
   * @berif set input target
   */
  void
  SetInputTarget(const std::shared_ptr<RenderTargetNode>& target) {
    const auto& targetName = target->GetTargetName();
    if (m_inputTarget.find(targetName) == m_inputTarget.cend()) {
      LOG(ERROR) << FORMAT("{} isn't set as an input target, won't set it", targetName);
      return;
    }
    m_inputTarget[targetName] = target;
  }

 protected:
  String m_inputPassNode;
  std::unordered_map<String, std::shared_ptr<RenderTargetNode>> m_inputTarget;
};

}  // namespace Marbas
