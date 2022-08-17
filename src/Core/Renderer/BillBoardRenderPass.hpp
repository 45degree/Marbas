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
  CreateBufferForEveryEntity(const entt::entity entity, const std::shared_ptr<Scene>& scene);

  /**
   * @brief record the command
   */
  void
  RecordCommand(const std::shared_ptr<Scene>& scene) override;

  void
  SetUniformBuffer(const std::shared_ptr<Scene>& scene);

  /**
   * @berif execute the render pass node after setting the render grpah
   */
  void
  Execute(const std::shared_ptr<Scene>& scene,
          const std::shared_ptr<ResourceManager>& resourceManager) override;

 private:
#pragma pack(push, 1)
  struct MatrixUniformBufferBlock {
    glm::mat4 view;
    glm::mat4 perspective;
  } m_matrixUniformBlock;

  struct CameraInfoUniformBufferBlock {
    glm::vec3 right;
    glm::vec3 up;
  } m_cameraUniformBlock;
#pragma pack(pop)

  std::shared_ptr<UniformBuffer> m_matrixUniformBuffer = nullptr;
  std::shared_ptr<UniformBuffer> m_cameraUniformBuffer = nullptr;
  Uid m_shaderId;
};

}  // namespace Marbas
