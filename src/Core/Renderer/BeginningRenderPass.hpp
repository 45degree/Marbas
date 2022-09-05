#pragma once

#include "Core/Renderer/DeferredRenderPass.hpp"

namespace Marbas {

struct BeginningRenderPassCreateInfo : public DeferredRenderPassCreateInfo {
  BeginningRenderPassCreateInfo();
};

class BeginningRenderPass final : public DeferredRenderPass {
 public:
  const static String renderPassName;
  const static String targetName;
  const static String depthTargetName;

 public:
  explicit BeginningRenderPass(const BeginningRenderPassCreateInfo& createInfo);

 public:
  void
  OnInit() override {}

  void
  CreateFrameBuffer() override;

  void
  CreateShader() override {}

  void
  CreateDescriptorSetLayout() override {}

  void
  RecordCommand(const Scene* scene) override;

  void
  CreatePipeline() override;

  void
  CreateRenderPass() override;

  virtual void
  Execute(const Scene* scene, const ResourceManager* resourceManager) override;
};

}  // namespace Marbas
