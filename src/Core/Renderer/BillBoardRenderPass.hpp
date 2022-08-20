#pragma once

#include "Core/Renderer/ForwardRenderPass.hpp"

namespace Marbas {

struct BillBoardRenderPassCreateInfo final : public ForwardRenderPassNodeCreateInfo {
  BillBoardRenderPassCreateInfo();
};

class BillBoardRenderPass final : public ForwardRenderPass {
 public:
  explicit BillBoardRenderPass(const BillBoardRenderPassCreateInfo& createInfo);
  virtual ~BillBoardRenderPass() = default;

 public:
  void
  CreateBufferForEveryEntity(const entt::entity entity, const Scene* scene);

  /**
   * @brief record the command
   */
  void
  RecordCommand(const Scene* scene) override;

  void
  SetUniformBuffer(const Scene* scene);

  /**
   * @berif execute the render pass node after setting the render grpah
   */
  void
  Execute(const Scene* scene, const ResourceManager* resourceManager) override;

 private:
  struct MatrixUniformBufferBlock {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 perspective;
  } m_matrixUniformBlock;

  struct CameraInfoUniformBufferBlock {
    alignas(16) glm::vec3 right;
    alignas(16) glm::vec3 up;
  } m_cameraUniformBlock;

  std::shared_ptr<UniformBuffer> m_matrixUniformBuffer = nullptr;
  std::shared_ptr<UniformBuffer> m_cameraUniformBuffer = nullptr;
  Uid m_shaderId;
};

}  // namespace Marbas
