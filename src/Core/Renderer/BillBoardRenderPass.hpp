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

  void
  GeneratePipeline() override;

 private:
  Uid m_shaderId;
};

}  // namespace Marbas