#pragma once

#include "Core/Renderer/DeferredRenderPass.hpp"

namespace Marbas {

struct BlinnPhongRenderPassCreateInfo : public DeferredRenderPassCreateInfo {
  BlinnPhongRenderPassCreateInfo();
};

class BlinnPhongRenderPass final : public DeferredRenderPass {
 public:
  static const String renderPassName;
  static const String blinnPhongTargetName;
  constexpr static uint32_t maxLightsCount = 20;

 public:
  explicit BlinnPhongRenderPass(const BlinnPhongRenderPassCreateInfo& createInfo);

 public:
  void
  CreateFrameBuffer() override;

  void
  GeneratePipeline() override;

  void
  RecordCommand(const Scene* scene) override;

  void
  Execute(const Scene* scene, const ResourceManager* resourceManager) override;

  void
  SetUniformBuffer(const Scene* scene);

 private:
  struct BlinnPhongLight {
    alignas(16) glm::vec3 pos = glm::vec3(0, 0, 0);
    alignas(16) glm::vec3 color = glm::vec3(1, 1, 1);
  };

  struct LightsInfo {
    alignas(16) BlinnPhongLight lights[maxLightsCount];
    alignas(16) uint32_t lightsCount = 0;
    alignas(16) glm::vec3 viewPos = glm::vec3(0, 0, 0);
  } m_uniformBufferBlock;

 private:
  Uid m_shaderId;
  std::shared_ptr<VertexBuffer> m_vertexBuffer;
  std::shared_ptr<UniformBuffer> m_lightUniformBuffer = nullptr;
  std::shared_ptr<DescriptorSet> m_descriptorSet;
};

}  // namespace Marbas
