#pragma once

#include "Common/Common.hpp"
#include "Core/Renderer/RenderTargetNode.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHI/RHI.hpp"
#include "Tool/Uid.hpp"

namespace Marbas {

struct RenderPassNodeCreateInfo {
  String passName;
  Vector<String> inputResource;
  Vector<String> outputResource;
  RHIFactory* rhiFactory;
  uint32_t width;
  uint32_t height;
};

class RenderPassNode {
 public:
  RenderPassNode(const RenderPassNodeCreateInfo& createInfo);

 public:
  const String&
  GetNodeName() const {
    return m_passName;
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

 public:
  /**
   * @berif create the frame buffer from the input and output target
   *
   * @warning it should be invoked after setting the target
   */
  virtual void
  CreateFrameBuffer() = 0;

  /**
   * @berif execute the render pass node after setting the render grpah
   */
  virtual void
  Execute(const std::shared_ptr<Scene>& scene,
          const std::shared_ptr<ResourceManager>& resourceManager) = 0;

 protected:
  String m_passName;
  uint32_t m_width;
  uint32_t m_height;
  std::shared_ptr<FrameBuffer> m_framebuffer = nullptr;
  std::shared_ptr<RenderPass> m_renderPass = nullptr;
  std::unordered_map<String, std::shared_ptr<RenderTargetNode>> m_inputTarget;
  std::unordered_map<String, std::shared_ptr<RenderTargetNode>> m_outputTarget;
  RHIFactory* m_rhiFactory = nullptr;
};

}  // namespace Marbas
