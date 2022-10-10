#pragma once

#include "Core/Renderer/DeferredRenderPass.hpp"

namespace Marbas {

struct IBLBRDFRenderPassCreateInfo final : public DeferredRenderPassCreateInfo {
  IBLBRDFRenderPassCreateInfo();
};

class IBLBRDFRenderPass final : public DeferredRenderPass {
 public:
  constexpr static StringView renderPassName = "IBL BRDF Render Pass";
  constexpr static StringView targetName = "IBL BRDF LOD";

 public:
  explicit IBLBRDFRenderPass(const IBLBRDFRenderPassCreateInfo& createInfo);
  virtual ~IBLBRDFRenderPass() = default;

 public:
  void
  OnInit() override;

  void
  CreateFrameBuffer() override;

  void
  CreateShader() override;

  void
  CreateRenderPass() override;

  void
  CreateDescriptorSetLayout() override;

  /**
   * @brief record the command
   */
  void
  RecordCommand(const Scene* scene) override;

  /**
   * @brief generate the pipeline
   */
  void
  CreatePipeline() override;

  void
  Execute(const Scene* scene, const ResourceManager* resourceMnager) override;

 private:
  std::shared_ptr<VertexBuffer> m_vertexBuffer;
  Uid m_shaderId;
};

}  // namespace Marbas
