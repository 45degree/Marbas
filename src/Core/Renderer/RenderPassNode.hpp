#pragma once

#include "Common/Common.hpp"
#include "Core/Renderer/RenderTargetNode.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHI/RHI.hpp"
#include "Tool/Uid.hpp"

namespace Marbas {

struct RenderPassNodeCreateInfo {
  String passName;
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
  RecordCommand(const Scene* scene) = 0;

  /**
   * @brief generate the pipeline
   */
  virtual void
  GeneratePipeline() = 0;

  /**
   * @berif execute the render pass node after setting the render grpah
   */
  virtual void
  Execute(const Scene* scene, const ResourceManager* resourceManager) = 0;

  void
  AddDescriptorSetLayoutBinding(const DescriptorSetLayoutBinding& bindingInfo);

  std::unique_ptr<DescriptorSet>
  GenerateDescriptorSet() {
    auto descriptorSet = m_rhiFactory->CreateDescriptorSet(m_descriptorSetLayout);
    descriptorSet->BindBuffer(0, m_cameraUniformBuffer);
    return descriptorSet;
  }

  void
  BindCameraUniformBuffer(DescriptorSet* descriptorSet) {
    descriptorSet->BindBuffer(0, m_cameraUniformBuffer);
  }

  void
  UpdateCameraUniformBuffer(Camera* camera) {
    m_cameraUniformBlock.view = camera->GetViewMatrix();
    m_cameraUniformBlock.projective = camera->GetProjectionMatrix();
    m_cameraUniformBlock.up = camera->GetUpVector();
    m_cameraUniformBlock.right = camera->GetRightVector();
    m_cameraUniformBlock.pos = camera->GetPosition();

    DLOG_ASSERT(m_cameraUniformBuffer != nullptr);
    m_cameraUniformBuffer->SetData(&m_cameraUniformBlock, sizeof(CameraUniformBlock), 0);
  }

 protected:
  String m_passName;
  uint32_t m_width;
  uint32_t m_height;
  bool m_needToRecordComand = true;

  struct CameraUniformBlock {
    alignas(16) glm::mat4 view = glm::mat4(1.0);
    alignas(16) glm::mat4 projective = glm::mat4(1.0);
    alignas(16) glm::vec3 right = glm::vec3(1, 0, 0);
    alignas(16) glm::vec3 up = glm::vec3(0, 1.0, 0);
    alignas(16) glm::vec3 pos = glm::vec3(0, 0, 0);
  } m_cameraUniformBlock;

  DescriptorSetLayout m_descriptorSetLayout;
  std::shared_ptr<UniformBuffer> m_cameraUniformBuffer = nullptr;

  std::shared_ptr<FrameBuffer> m_framebuffer = nullptr;
  std::shared_ptr<RenderPass> m_renderPass = nullptr;
  std::shared_ptr<GraphicsPipeLine> m_pipeline = nullptr;
  // std::shared_ptr<DescriptorSet> m_descriptorSet = nullptr;  // TODO: remove?
  std::shared_ptr<CommandFactory> m_commandFactory = nullptr;
  std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;
  std::shared_ptr<ResourceManager> m_resourceManager = nullptr;
  RHIFactory* m_rhiFactory = nullptr;
};

}  // namespace Marbas
