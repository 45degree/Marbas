#pragma once

#include "Core/Renderer/ForwardRenderPass.hpp"

namespace Marbas {

struct GridRenderPassCreateInfo : public ForwardRenderPassNodeCreateInfo {
  GridRenderPassCreateInfo();
};

class GridRenderPass final : public ForwardRenderPass {
 public:
  explicit GridRenderPass(const GridRenderPassCreateInfo& createInfo);
  virtual ~GridRenderPass() = default;

 public:
  /**
   * @brief record the command
   */
  void
  RecordCommand(const Scene* scene) override;

  void
  CreateRenderPass() override;

  void
  CreateShader() override;

  void
  CreateDescriptorSetLayout() override {}

  void
  OnInit() override;

  /**
   * @berif execute the render pass node after setting the render grpah
   */
  void
  Execute(const Scene* scene, const ResourceManager* resourceManager) override;

  void
  SetUniformBuffer(const Scene* scene);

  void
  CreatePipeline() override;

 private:
  std::shared_ptr<UniformBuffer> m_uniformBuffer = nullptr;
  std::shared_ptr<DescriptorSet> m_descriptorSet = nullptr;
  std::shared_ptr<VertexBuffer> m_vertexBuffer = nullptr;

  Uid m_shaderId;
};

}  // namespace Marbas
