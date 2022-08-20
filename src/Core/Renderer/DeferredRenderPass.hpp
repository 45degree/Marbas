#pragma once

#include "Core/Renderer/RenderPassNode.hpp"

namespace Marbas {

struct DeferredRenderPassCreateInfo : public RenderPassNodeCreateInfo {
  Vector<String> inputResource;
  Vector<String> outputResource;
};

class DeferredRenderPass : public RenderPassNode {
 public:
  explicit DeferredRenderPass(const DeferredRenderPassCreateInfo& createInfo);
  virtual ~DeferredRenderPass() = default;

 public:
  /**
   * @berif create the frame buffer from the input and output target
   *
   * @warning it should be invoked after setting the target
   */
  virtual void
  CreateFrameBuffer() = 0;

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
   * @berif get all output render target name
   */
  Vector<String>
  GetAllOutputTargetName() const {
    Vector<String> res;
    for (const auto& [name, resource] : m_outputTarget) {
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

  /**
   * @berif set output target
   */
  void
  SetOutputTarget(const std::shared_ptr<RenderTargetNode>& target) {
    const auto& targetName = target->GetTargetName();
    if (m_outputTarget.find(targetName) == m_outputTarget.cend()) {
      LOG(ERROR) << FORMAT("{} isn't set as an ouput target, won't set it", targetName);
      return;
    }
    m_outputTarget[targetName] = target;
  }

 protected:
  std::unordered_map<String, std::shared_ptr<RenderTargetNode>> m_inputTarget;
  std::unordered_map<String, std::shared_ptr<RenderTargetNode>> m_outputTarget;
};

}  // namespace Marbas
