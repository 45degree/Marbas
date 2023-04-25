#pragma once

#include <entt/entt.hpp>

#include "Core/Renderer/RenderGraph/RenderGraphResourceManager.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

struct RenderInfo final {
  Scene* scene;
  int imageIndex;
  Semaphore* waitSemaphore = nullptr;
  Semaphore* signalSemaphore = nullptr;
  Fence* fence = nullptr;
};

struct RenderSystem final {
  static void
  Initialize(RHIFactory* rhiFactory);

  static void
  CreateRenderGraph(Scene* scene, RHIFactory* rhiFactory);

  static void
  Update(const RenderInfo& renderInfo);

  static ImageView*
  GetOutputView();

  static void
  Destroy(RHIFactory* rhiFactory);

  static void
  RerunPreComputePass(const StringView& passName, RHIFactory* rhiFactory);

  template <auto Func, typename Type>
  static void
  RegistryListenerForResultImageChange(Type&& instance) {
    s_sink.connect<Func>(std::forward<Type>(instance));
  }

 private:
  /**
   * @brief Update the position and size of each shadow map in the texture atlas
   *
   * @param scene scene
   */
  static void
  UpdateShadowMapAtlasPosition(Scene* scene);

 private:
  static std::shared_ptr<RenderGraphResourceManager> s_resourceManager;
  static std::unique_ptr<RenderGraph> s_renderGraph;
  static std::unique_ptr<RenderGraph> s_precomputeRenderGraph;
  static Fence* s_precomputeFence;
  static ImageView* s_resultImageView;

  static entt::sigh<void(ImageView*)> s_newResultImageViewEvent;
  static entt::sink<entt::sigh<void(ImageView*)>> s_sink;
};

}  // namespace Marbas
