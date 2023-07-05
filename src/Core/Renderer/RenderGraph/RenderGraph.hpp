#pragma once

#include "Common/Common.hpp"
#include "RHIFactory.hpp"
#include "RenderGraphPass.hpp"
#include "RenderGraphResourceManager.hpp"

namespace Marbas {

class RenderGraph final {
 public:
  /**
   * @brief create a render graph
   *
   * @param rhiFactory rhi factory
   * @param resourceManager render graph resource manager
   * @param fifCount frame in flight count
   */
  RenderGraph(RHIFactory* rhiFactory, std::shared_ptr<RenderGraphResourceManager>& resourceManager, int fifCount = 1)
      : m_rhiFactory(rhiFactory), m_resourceManager(resourceManager) {}
  ~RenderGraph() {
    for (auto pass : m_passes) {
      delete pass;
    }
  }

 public:
  template <details::RenderGraphGraphicsLambdaPass SetupLambda>
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

  template <details::RenderGraphComputeLambdaPass SetupLambda>
  void
  AddPass(std::string_view name, const SetupLambda& lambda, std::function<bool()> func = nullptr) {
    auto* pass = new details::LambdaComputeRenderGraphPass(name, m_rhiFactory);
    RenderGraphComputeBuilder builder(pass, this);
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

  template <details::RenderGraphComputeStructPass StructPass, typename... Args>
  void
  AddPass(StringView name, Args&&... args) {
    auto* pass =
        new details::StructRenderGraphComputePass<StructPass, Args...>(name, m_rhiFactory, std::forward<Args>(args)...);
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
  Execute(Semaphore* waitSemaphore = nullptr, Semaphore* signalSemaphore = nullptr, Fence* fence = nullptr,
          void* userData = nullptr);

  void
  ExecuteAlone(const StringView& passName, Semaphore* waitSemaphore = nullptr, Semaphore* signalSemaphore = nullptr,
               Fence* fence = nullptr, void* userData = nullptr);

 private:
  friend class RenderGraphGraphicsRegistry;
  friend class RenderGraphGraphicsBuilder;
  friend class RenderGraphComputeRegistry;
  friend class RenderGraphComputeBuilder;
  friend class details::RenderGraphGraphicsPass;
  friend class details::ImageDesc;

  RHIFactory* m_rhiFactory;

  Vector<details::RenderGraphPass*> m_passes;

  // the semaphores pool, it means the min semaphore count if you want execute all pass.
  Vector<Semaphore*> m_semaphores;

  std::shared_ptr<RenderGraphResourceManager> m_resourceManager;
};

}  // namespace Marbas
