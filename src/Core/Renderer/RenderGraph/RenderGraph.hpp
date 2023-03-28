#pragma once

#include "Common/Common.hpp"
#include "RHIFactory.hpp"
#include "RenderGraphPass.hpp"
#include "RenderGraphResourceManager.hpp"

namespace Marbas {

class RenderGraph final {
 public:
  RenderGraph(RHIFactory* rhiFactory, std::shared_ptr<RenderGraphResourceManager>& resourceManager)
      : m_rhiFactory(rhiFactory), m_resourceManager(resourceManager) {}
  ~RenderGraph() {
    for (auto pass : m_passes) {
      delete pass;
    }
  }

 public:
  template <details::RenderGraphLambdaPass SetupLambda>
  void
  AddPass(const char* name, const SetupLambda& lambda, std::function<bool()> func = nullptr) {
    auto* pass = new details::LambdaGraphicsRenderGraphPass(name, m_rhiFactory);
    RenderGraphGraphicsBuilder builder(pass, this);
    const auto& execute = lambda(builder);
    pass->SetRecordCommand(execute);
    if (func != nullptr) {
      pass->SetEnableFunc(func);
    }
    m_passes.push_back(pass);
  }

  template <details::RenderGraphGraphicsStructPass StructPass, typename... Args>
  void
  AddPass(StringView name, Args&&... args) {
    auto* pass =
        new details::StructRenderGraphPass<StructPass, Args...>(name, m_rhiFactory, std::forward<Args>(args)...);
    pass->SetUp(this);
    m_passes.push_back(pass);
  }

  void
  ClearAllPass() {
    for (auto* pass : m_passes) {
      delete pass;
    }
    m_passes.clear();
  }

  void
  Compile();

  void
  Execute(Semaphore* waitSemaphore = nullptr, Semaphore* signalSemaphore = nullptr, Fence* fence = nullptr);

  void
  ExecuteAlone(const StringView& passName, Semaphore* waitSemaphore = nullptr, Semaphore* signalSemaphore = nullptr,
               Fence* fence = nullptr);

 private:
  friend class RenderGraphRegistry;
  friend class RenderGraphGraphicsBuilder;
  friend class details::RenderGraphGraphicsPass;

  RHIFactory* m_rhiFactory;

  Vector<details::RenderGraphPass*> m_passes;

  // the semaphores pool, it means the min semaphore count if you want execute all pass.
  Vector<Semaphore*> m_semaphores;

  std::shared_ptr<RenderGraphResourceManager> m_resourceManager;
};

}  // namespace Marbas
