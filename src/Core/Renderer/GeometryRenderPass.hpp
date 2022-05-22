#pragma once

#include "Core/Renderer/RenderPassNode.hpp"
#include "Core/Scene/Component/MeshComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"
#include "Core/Scene/Entity/MeshEntity.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

struct GeometryRenderPassCreatInfo : public RenderPassNodeCreateInfo {
  // std::shared_ptr<IResourceContainer<MaterialResource>> materialResourceContainer;
  std::shared_ptr<ResourceManager> resourceManager;
  GeometryRenderPassCreatInfo();
};

class GeometryRenderPass final : public RenderPassNode {
  using ResourceContainerInfo = std::tuple<MaterialResource>;

 public:
  const static String geometryTargetName;
  const static String depthTargetName;

 public:
  explicit GeometryRenderPass(const GeometryRenderPassCreatInfo& createInfo);

 private:
  std::shared_ptr<CommandFactory> m_commandFactory = nullptr;
  std::shared_ptr<GraphicsPipeLine> m_pipeline = nullptr;
  std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;
  std::shared_ptr<DynamicDescriptorSet> m_dynamicDescriptorSet = nullptr;

  // this buffer used to store all mesh's MVP matrix
  std::shared_ptr<DynamicUniformBuffer> m_dynamicUniforBuffer = nullptr;
  // std::shared_ptr<IResourceContainer<MaterialResource>> m_materialResourceContainer = nullptr;
  std::shared_ptr<ResourceManager> m_resourceManager = nullptr;
  bool m_needToRecordComand = false;

 public:
  void
  CreateFrameBuffer() override;

  /**
   * @brief record the command
   */
  void
  RecordCommand(const std::shared_ptr<Scene>& scene);

  void
  CreateBufferForEveryEntity(const MeshEntity& mesh, const std::shared_ptr<Scene>& scene);

  void
  SetUniformBuffer(const std::shared_ptr<Scene>& scene);

  void
  Execute(const std::shared_ptr<Scene>& scene,
          const std::shared_ptr<ResourceManager>& resourceMnager) override;
};

}  // namespace Marbas
