#pragma once

#include "Common/Light.hpp"
#include "Core/Renderer/DeferredRenderPass.hpp"

namespace Marbas {

struct PointLightShadowMappingRenderPassCreateInfo final : public DeferredRenderPassCreateInfo {
  PointLightShadowMappingRenderPassCreateInfo();
};

class PointLightShadowMappingRenderPass final : public DeferredRenderPass {
  using CreateInfo = PointLightShadowMappingRenderPassCreateInfo;

 public:
  static constexpr int MAX_LIGHT_COUNT = 3;
  static constexpr StringView renderPassName = "point light shadow mapping";
  static constexpr StringView targetName = "point light shadow";

 public:
  explicit PointLightShadowMappingRenderPass(const CreateInfo& createInfo);
  virtual ~PointLightShadowMappingRenderPass() = default;

 public:
  void
  OnInit() override;

  void
  Execute(const Scene* scene, const ResourceManager* resourceManager) override;

  void
  CreatePipeline() override;

  void
  CreateRenderPass() override;

  void
  CreateDescriptorSetLayout() override;

  void
  CreateShader() override;

  /**
   * @berif create the frame buffer from the input and output target
   *
   * @warning it should be invoked after setting the target
   */
  void
  CreateFrameBuffer() override;

  /**
   * @brief record the command
   */
  void
  RecordCommand(const Scene* scene) override;

  void
  CreateBufferForEveryEntity(const entt::entity& entity, Scene* scene);

  void
  SetUniformBuffer(const Scene* scene);

  void
  SetUniformBufferForLight(const Scene* scene, int lightIndex);

 private:
  struct LightInfo {
    alignas(16) glm::vec3 pos = glm::vec3(0, 0, 0);
    alignas(4) float farPlane = 25.0;
    alignas(16) glm::mat4 matrixes[6];
    alignas(16) glm::mat4 projectMatrix;
    alignas(4) int lightIndex = 0;
  } m_lightInfos;

  Uid m_shaderId;

  bool m_needToRecordCopyCommand = true;
  std::shared_ptr<DynamicUniformBuffer> m_meshDynamicUniformBuffer = nullptr;
  std::shared_ptr<UniformBuffer> m_lightInfoUniformBuffer = nullptr;
};

}  // namespace Marbas
