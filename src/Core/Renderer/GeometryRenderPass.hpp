#pragma once

#include "Core/Renderer/DeferredRenderPass.hpp"
#include "Core/Renderer/RenderPassNode.hpp"
#include "Core/Scene/Component/MeshComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"
#include "Core/Scene/Entity/MeshEntity.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

struct GeometryRenderPassCreatInfo final : public DeferredRenderPassNodeCreateInfo {
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
  CreateFrameBuffer() override;

  /**
   * @brief record the command
   */
  void
  RecordCommand(const std::shared_ptr<Scene>& scene) override;

  void
  CreateBufferForEveryEntity(const MeshEntity& mesh, const std::shared_ptr<Scene>& scene);

  void
  SetUniformBuffer(const std::shared_ptr<Scene>& scene);

  void
  Execute(const std::shared_ptr<Scene>& scene,
          const std::shared_ptr<ResourceManager>& resourceMnager) override;

 private:
  std::shared_ptr<DynamicDescriptorSet> m_dynamicDescriptorSet = nullptr;

  // this buffer used to store all mesh's MVP matrix
  std::shared_ptr<DynamicUniformBuffer> m_dynamicUniforBuffer = nullptr;
};

}  // namespace Marbas
