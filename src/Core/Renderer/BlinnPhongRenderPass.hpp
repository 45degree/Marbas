#pragma once

#include "Core/Renderer/DeferredRenderPass.hpp"
#include "Core/Renderer/DirectionLightShadowMapRenderPass.hpp"
#include "Core/Renderer/PointLightShadowMapRenderPass.hpp"

namespace Marbas {

struct BlinnPhongRenderPassCreateInfo : public DeferredRenderPassCreateInfo {
  BlinnPhongRenderPassCreateInfo();
};

class BlinnPhongRenderPass final : public DeferredRenderPass {
 public:
  static const String renderPassName;
  static const String blinnPhongTargetName;

 public:
  explicit BlinnPhongRenderPass(const BlinnPhongRenderPassCreateInfo& createInfo);

 public:
  void
  OnInit() override;

  void
  CreateRenderPass() override;

  void
  CreateFrameBuffer() override;

  void
  CreateShader() override;

  void
  CreateDescriptorSetLayout() override;

  void
  CreatePipeline() override;

  void
  RecordCommand(const Scene* scene) override;

  void
  Execute(const Scene* scene, const ResourceManager* resourceManager) override;

  void
  SetUniformBuffer(const Scene* scene);

 private:
  struct PointLight {
    alignas(16) glm::vec3 pos = glm::vec3(0, 0, 0);
    alignas(16) glm::vec3 color = glm::vec3(1, 1, 1);
    alignas(4) float farPlane = 100.0;
  };

  struct PointLightInfoBlock {
    alignas(16) PointLight lights[PointLightShadowMapRenderPass::MAX_LIGHT_COUNT];
    alignas(4) uint32_t pointLightCount = 0;
  } m_pointLightUniformBlock;

  struct DirectionLight {
    alignas(16) glm::vec3 pos = glm::vec3(0, 0, 0);
    alignas(16) glm::vec3 color = glm::vec3(1, 1, 1);
    alignas(16) glm::vec3 direction = glm::vec3(0, 0, -1);
    alignas(16) glm::mat4 view = glm::mat4(1);
    alignas(16) glm::mat4 projection = glm::mat4(1);
  };

  struct DirectionLightBlock {
    alignas(16) DirectionLight lights[DirectionLightShadowMapRenderPass::MAX_LIGHT_COUNT];
    alignas(4) uint32_t lightCount = 0;
  } m_dirLightUboBlock;

 private:
  Uid m_shaderId;
  std::shared_ptr<VertexBuffer> m_vertexBuffer;
  std::shared_ptr<UniformBuffer> m_pointLightUniformBuffer = nullptr;
  std::shared_ptr<UniformBuffer> m_dirLightUbo = nullptr;
  std::shared_ptr<DescriptorSet> m_descriptorSet;
};

}  // namespace Marbas
