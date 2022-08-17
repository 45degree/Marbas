#pragma once

#include "Core/Renderer/DeferredRenderPass.hpp"

namespace Marbas {

struct BeginningRenderPassCreateInfo : public DeferredRenderPassNodeCreateInfo {
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
  RecordCommand(const std::shared_ptr<Scene>& scene) override;

  virtual void
  Execute(const std::shared_ptr<Scene>& scene,
          const std::shared_ptr<ResourceManager>& resourceManager) override;
};

}  // namespace Marbas
