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
  constexpr static StringView renderPassName = "shadow mapping render pass";
  constexpr static StringView renderTarget = "shadow mapping render target";

 public:
  explicit ShadowMappingRenderPass(const ShadowMappingCreateInfo& createInfo);
  virtual ~ShadowMappingRenderPass() = default;

 public:
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
  GeneratePipeline() override;

  void
  SetUniformBuffer(const Scene* scene, const ParallelLight& light);

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
    alignas(16) glm::mat4 view = glm::mat4(1.0);
    alignas(16) glm::mat4 projective = glm::mat4(1.0);
  } m_lightUniformBlock;

  struct LightInfo {
    alignas(16) glm::vec3 pos = glm::vec3(0, 0, 0);
  } m_lightInfo;

  Uid m_depthShaderId;
  Uid m_shaderId;

  std::shared_ptr<RenderPass> m_depthRenderPass = nullptr;
  std::shared_ptr<GraphicsPipeLine> m_depthPipeline = nullptr;
  std::shared_ptr<CommandBuffer> m_depthCommandBuffer = nullptr;
  std::shared_ptr<FrameBuffer> m_depthFrameBuffer = nullptr;
  std::shared_ptr<Texture2D> m_depthTexture = nullptr;
  std::shared_ptr<DynamicUniformBuffer> m_meshDynamicUniformBuffer = nullptr;
  std::shared_ptr<UniformBuffer> m_lightUniformBuffer = nullptr;
  std::shared_ptr<UniformBuffer> m_lightInfoUniformBuffer = nullptr;
  DescriptorSetLayout m_depthDescriptorSetLayout;

  std::shared_ptr<VertexBuffer> m_vertexBuffer;
  std::shared_ptr<DescriptorSet> m_shadowDescriptorSet;
};

}  // namespace Marbas
