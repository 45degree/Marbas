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
  CreateFrameBuffer() override;

  void
  RecordCommand(const Scene* scene) override;

  void
  GeneratePipeline() override;

  virtual void
  Execute(const Scene* scene, const ResourceManager* resourceManager) override;
};

}  // namespace Marbas