#pragma once

#include "Common/Light.hpp"
#include "Core/Renderer/DeferredRenderPass.hpp"
#include "entt/entity/fwd.hpp"

namespace Marbas {

struct ShadowMappingCreateInfo : public DeferredRenderPassCreateInfo {
  ShadowMappingCreateInfo();
};

class ShadowMappingRenderPass : public DeferredRenderPass {
 public:
  static constexpr int MAX_LIGHT_COUNT = 3;
  constexpr static StringView renderPassName = "shadow mapping render pass";
  constexpr static StringView renderTarget = "shadow mapping render target";

 public:
  explicit ShadowMappingRenderPass(const ShadowMappingCreateInfo& createInfo);
  virtual ~ShadowMappingRenderPass() = default;

 public:
  void
  OnInit() override;

  void
  CreateShader() override;

  void
  CreateRenderPass() override;

  void
  CreateDescriptorSetLayout() override;

  void
  CreatePipeline() override;

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

  /**
   * @berif execute the render pass node after setting the render grpah
   */
  void
  Execute(const Scene* scene, const ResourceManager* resourceManager) override;

  void
  SetUniformBuffer(const Scene* scene);

  void
  SetUniformBufferForLight(const Scene* scene, int lightIndex);

  /**
   * @brief set descirptor set for every mesh that enable shadow
   *
   * @param entity
   * @param scene
   */
  void
  CreateBufferForEveryEntity(const entt::entity& entity, Scene* scene);

 private:
  struct LightUniformBlock {
    alignas(4) int lightIndex;
    alignas(16) glm::mat4 view = glm::mat4(1.0);
    alignas(16) glm::mat4 projective = glm::mat4(1.0);
  } m_lightUniformBlock;

  Uid m_shaderId;

  std::shared_ptr<DynamicUniformBuffer> m_meshDynamicUniformBuffer = nullptr;
  std::shared_ptr<UniformBuffer> m_lightUniformBuffer = nullptr;
};

}  // namespace Marbas
