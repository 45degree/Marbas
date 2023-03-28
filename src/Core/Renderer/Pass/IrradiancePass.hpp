#pragma once

#include "Core/Renderer/RenderPassBase.hpp"

namespace Marbas {

class IrradiancePass final : public RenderPassBase {
 public:
  explicit IrradiancePass(const String& passName, ResourceManager* resMgr, GBufferRegistry* gBufReg,
                          RHIFactory* rhiFactory);
  virtual ~IrradiancePass() = default;

  IrradiancePass(const IrradiancePass&) = delete;

  IrradiancePass&
  operator=(const IrradiancePass&) = delete;

 protected:
  Pipeline*
  CreatePipeline(PipelineContext* pipelineContext) override;

  void
  SetFrameBuffer(FrameBufferCreateInfo& createInfo) override;

  void
  RecordCommandBuffer(Scene* scene) override;

  void
  SubmitCommand(std::span<Semaphore*> waitSemaphore, std::span<Semaphore*> signalSemaphore) override;

 private:
  void
  CreateShader();

 private:
  struct ViewMatrix {
    glm::mat4 viewMatrix[6];
    glm::mat4 projectionMatrix;
  } m_viewMatrix;

  Uid m_shaderId;
  ResourceManager* m_resourceManager;
};
}  // namespace Marbas
