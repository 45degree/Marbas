#pragma once

#include "Common/Common.hpp"
#include "Core/Renderer/RenderTargetNode.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHI/RHI.hpp"
#include "Tool/Uid.hpp"

#ifdef _WIN32
#undef near
#undef far
#endif

namespace Marbas {

struct RenderPassNodeCreateInfo {
  String passName;
  std::shared_ptr<ResourceManager> resourceManager;
  RHIFactory* rhiFactory = nullptr;
  uint32_t width = 800;
  uint32_t height = 600;
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
  virtual void
  Initialize() = 0;

  virtual void
  OnInit() = 0;

  virtual void
  CreateRenderPass() = 0;

  virtual void
  CreateShader() = 0;

  virtual void
  CreateDescriptorSetLayout() = 0;

  void
  ClearDepth() {
    m_clearCommandBuffer->SubmitCommand();
  }

  void
  RecordClearDepthCommand();

  /**
   * @brief record the command
   */
  virtual void
  RecordCommand(const Scene* scene) = 0;

  /**
   * @brief generate the pipeline
   */
  virtual void
  CreatePipeline() = 0;

  /**
   * @berif execute the render pass node after setting the render grpah
   */
  virtual void
  Execute(const Scene* scene, const ResourceManager* resourceManager) = 0;

  void
  AddDescriptorSetLayoutBinding(const DescriptorSetLayoutBinding& bindingInfo);

  void
  UpdateCameraUniformBuffer(Camera* camera) {
    m_cameraUniformBlock.view = camera->GetViewMatrix();
    m_cameraUniformBlock.projective = camera->GetProjectionMatrix();
    m_cameraUniformBlock.up = camera->GetUpVector();
    m_cameraUniformBlock.right = camera->GetRightVector();
    m_cameraUniformBlock.pos = camera->GetPosition();
    m_cameraUniformBlock.far = camera->GetFar();
    m_cameraUniformBlock.near = camera->GetNear();

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
    alignas(4) float far = 100;
    alignas(4) float near = 0.1;
  } m_cameraUniformBlock;

  DescriptorSetLayout m_descriptorSetLayout;
  std::shared_ptr<UniformBuffer> m_cameraUniformBuffer = nullptr;

  std::shared_ptr<FrameBuffer> m_framebuffer = nullptr;
  std::shared_ptr<RenderPass> m_renderPass = nullptr;
  std::shared_ptr<GraphicsPipeLine> m_pipeline = nullptr;
  std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;
  std::shared_ptr<ResourceManager> m_resourceManager = nullptr;

  std::shared_ptr<RenderPass> m_clearDepthRenderPass = nullptr;
  std::shared_ptr<CommandBuffer> m_clearCommandBuffer = nullptr;

  RHIFactory* m_rhiFactory = nullptr;
};

}  // namespace Marbas
