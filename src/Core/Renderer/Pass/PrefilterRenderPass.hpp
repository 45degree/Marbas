#pragma once

#include "Core/Renderer/DeferredRenderPass.hpp"

namespace Marbas {

class PrefilterRenderPassCreateInfo final : public DeferredRenderPassCreateInfo {
 public:
  PrefilterRenderPassCreateInfo();
};

class PrefilterRenderPass final : public DeferredRenderPass {
 public:
  static constexpr StringView renderPassName = "Prefilter Render Pass";
  static constexpr StringView targetName = "Prefilter target";

 public:
  explicit PrefilterRenderPass(const PrefilterRenderPassCreateInfo& createInfo);
  ~PrefilterRenderPass() = default;

  PrefilterRenderPass(const PrefilterRenderPass&) = delete;
  PrefilterRenderPass&
  operator=(const PrefilterRenderPass&) = delete;

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
    alignas(16) glm::mat4 viewMatrix[6];
    alignas(16) glm::mat4 projectionMatrix;
    alignas(4) float roughness;
  } m_viewMatrix;

  constexpr static uint32_t maxMipmapLevel = 5;
  std::array<std::shared_ptr<FrameBuffer>, maxMipmapLevel> m_frameBuffers;
  std::array<std::shared_ptr<ImageView>, maxMipmapLevel> m_frameBufferImageViews;
  std::array<std::shared_ptr<CommandBuffer>, maxMipmapLevel> m_commandBuffers;

  Uid m_shaderId;
  std::shared_ptr<UniformBuffer> m_ViewMatrixUBO = nullptr;
  std::shared_ptr<VertexBuffer> m_vertexBuffer = nullptr;
  std::shared_ptr<IndexBuffer> m_indexBuffer = nullptr;
  std::shared_ptr<DescriptorSet> m_descriptorSet = nullptr;
};

}  // namespace Marbas
