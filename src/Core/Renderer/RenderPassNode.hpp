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
  std::shared_ptr<ResourceManager> resourceManager;
  RHIFactory* rhiFactory;
  uint32_t width;
  uint32_t height;
};

class RenderPassNode {
 public:
  RenderPassNode(const RenderPassNodeCreateInfo& createInfo);
  virtual ~RenderPassNode() = default;

 public:
  const String&
  GetNodeName() const {
    return m_passName;
  }

  std::shared_ptr<FrameBuffer>
  GetFrameBuffer() {
    return m_framebuffer;
  }

 public:
  /**
   * @brief record the command
   */
  virtual void
  RecordCommand(const std::shared_ptr<Scene>& scene) = 0;

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
  bool m_needToRecordComand = false;
  std::shared_ptr<FrameBuffer> m_framebuffer = nullptr;
  std::shared_ptr<RenderPass> m_renderPass = nullptr;
  std::shared_ptr<CommandFactory> m_commandFactory = nullptr;
  std::shared_ptr<GraphicsPipeLine> m_pipeline = nullptr;
  std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;
  std::shared_ptr<ResourceManager> m_resourceManager = nullptr;
  RHIFactory* m_rhiFactory = nullptr;
};

}  // namespace Marbas
