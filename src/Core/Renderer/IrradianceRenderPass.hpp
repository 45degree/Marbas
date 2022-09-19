#pragma once

#include "Core/Renderer/DeferredRenderPass.hpp"

namespace Marbas {

struct IrradianceRenderPassCreateInfo : public DeferredRenderPassCreateInfo {
  IrradianceRenderPassCreateInfo();
};

class IrradianceRenderPass final : public DeferredRenderPass {
 public:
  static constexpr StringView renderPassName = "Irradiance Render Pass";
  static constexpr StringView targetName = "Irradiance Image target";

 public:
  explicit IrradianceRenderPass(const IrradianceRenderPassCreateInfo& createInfo);
  virtual ~IrradianceRenderPass() = default;

  IrradianceRenderPass(const IrradianceRenderPass&) = delete;
  IrradianceRenderPass&
  operator=(const IrradianceRenderPass&) = delete;

 public:
  void
  CreateFrameBuffer() override;

  void
  OnInit() override;

  void
  CreateRenderPass() override;

  void
  CreateShader() override;

  void
  CreateDescriptorSetLayout() override;

  void
  RecordCommand(const Scene* scene) override;

  void
  CreatePipeline() override;

  void
  SetUniformBuffer(Scene* scene);

  void
  Execute(const Scene* scene, const ResourceManager* resourceManager) override;

 private:
  struct ViewMatrix {
    glm::mat4 viewMatrix[6];
    glm::mat4 projectionMatrix;
  } m_viewMatrix;

  Uid m_shaderId;
  std::shared_ptr<UniformBuffer> m_ViewMatrixUBO = nullptr;
  std::shared_ptr<VertexBuffer> m_vertexBuffer = nullptr;
  std::shared_ptr<IndexBuffer> m_indexBuffer = nullptr;
  std::shared_ptr<DescriptorSet> m_descriptorSet = nullptr;
};
}  // namespace Marbas
