#pragma once

#include "Core/Renderer/DeferredRenderPass.hpp"
#include "Core/Renderer/RenderPassNode.hpp"
#include "Core/Scene/Component/MeshComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"
#include "Core/Scene/Entity/MeshEntity.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

struct GeometryRenderPassCreatInfo final : public DeferredRenderPassCreateInfo {
  GeometryRenderPassCreatInfo();
};

class GeometryRenderPass final : public DeferredRenderPass {
 public:
  const static String renderPassName;
  const static String geometryTargetName;
  const static String depthTargetName;

 public:
  explicit GeometryRenderPass(const GeometryRenderPassCreatInfo& createInfo);
  virtual ~GeometryRenderPass() = default;

 public:
  void
  OnInit() override {}

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
  CreateBufferForEveryEntity(const MeshEntity& mesh, Scene* scene);

  void
  SetUniformBuffer(const Scene* scene);

  void
  Execute(const Scene* scene, const ResourceManager* resourceMnager) override;

 private:
  // this buffer used to store all mesh's MVP matrix
  std::shared_ptr<DynamicUniformBuffer> m_dynamicUniforBuffer = nullptr;

  Uid m_shaderId;
};

}  // namespace Marbas
