#pragma once

#include "Core/Renderer/ForwardRenderPass.hpp"
#include "Core/Renderer/RenderPassNode.hpp"
#include "Core/Scene/Entity/CubeMapEntity.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

struct CubeMapRenderPassCreateInfo final : public ForwardRenderPassNodeCreateInfo {
  CubeMapRenderPassCreateInfo();
};

class CubeMapRenderPass final : public ForwardRenderPass {
 public:
  explicit CubeMapRenderPass(const CubeMapRenderPassCreateInfo& createInfo);
  virtual ~CubeMapRenderPass() = default;

 public:
  void
  RecordCommand(const Scene* scene) override;

  void
  CreateBufferForEveryEntity(const entt::entity entity, Scene* scene);

  void
  SetUniformBuffer(Scene* scene);

  void
  Execute(const Scene* scene, const ResourceManager* resourceMnager) override;

 private:
  std::shared_ptr<UniformBuffer> m_uniformBuffer = nullptr;
  Uid m_shaderId;
};

}  // namespace Marbas
