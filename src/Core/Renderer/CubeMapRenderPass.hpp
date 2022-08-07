#pragma once

#include "Core/Renderer/ForwardRenderPass.hpp"
#include "Core/Renderer/RenderPassNode.hpp"
#include "Core/Scene/Entity/CubeMapEntity.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

struct CubeMapRenderPassCreateInfo final : public RenderPassNodeCreateInfo {
  CubeMapRenderPassCreateInfo();
};

class CubeMapRenderPass final : public ForwardRenderPass {
 public:
  explicit CubeMapRenderPass(const CubeMapRenderPassCreateInfo& createInfo);
  virtual ~CubeMapRenderPass() = default;

 public:
  void
  RecordCommand(const std::shared_ptr<Scene>& scene) override;

  void
  CreateBufferForEveryEntity(const entt::entity entity, const std::shared_ptr<Scene>& scene);

  void
  SetUniformBuffer(const std::shared_ptr<Scene>& scene);

  void
  Execute(const std::shared_ptr<Scene>& scene,
          const std::shared_ptr<ResourceManager>& resourceMnager) override;

 private:
  std::shared_ptr<UniformBuffer> m_uniformBuffer = nullptr;
};

}  // namespace Marbas
