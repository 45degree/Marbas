#include "RenderLightDataJob.hpp"

#include "Core/Scene/Component/RenderComponent/LightRenderComponent.hpp"
#include "Core/Scene/System/RenderSystemJob/RenderSystem.hpp"

namespace Marbas::Job {

void
RenderLightDataJob::update(DeltaTime deltaTime, void* data) {
  auto* renderInfo = reinterpret_cast<RenderInfo*>(data);
  auto* scene = reinterpret_cast<Job::RenderUserData*>(renderInfo->userData)->scene;
  auto& world = scene->GetWorld();
  auto rootEntity = scene->GetRootNode();

  LOG_IF(WARNING, world.view<LightRenderComponent>().size() > 1) << "";
  if (!world.any_of<LightRenderComponent>(rootEntity)) {
    scene->Emplace<LightRenderComponent>(rootEntity, m_rhiFactory);
  }

  // TODO: remove other light render component

  UpdateDirectionShadowInfo(scene);
  UpdateShadowMapAtlasPosition(scene);

  auto shadowDirLightView = world.view<DirectionLightComponent, DirectionShadowComponent>();
  auto dirLightView = world.view<DirectionLightComponent>(entt::exclude<DirectionShadowComponent>);
  scene->Update<LightRenderComponent>(rootEntity, [&](auto&& component) {
    for (auto&& [entity, light, shadow] : shadowDirLightView.each()) {
      if (!light.lightIndex) {
        scene->Update<DirectionLightComponent>(entity, [&](auto& light) {
          component.AddLight(light);
          return true;
        });
      }
      component.UpdateLight(light, shadow);
    }

    for (auto&& [entity, light] : dirLightView.each()) {
      if (!light.lightIndex) {
        scene->Update<DirectionLightComponent>(entity, [&](auto& light) {
          component.AddLight(light);
          return true;
        });
      }
      component.UpdateLight(light);
    }
    return true;
  });
}

void
RenderLightDataJob::UpdateDirectionShadowInfo(Scene* scene) {
  auto& world = scene->GetWorld();
  auto camera = scene->GetEditorCamrea();
  auto view = world.view<DirectionShadowComponent, DirectionLightComponent>();
  for (auto&& [entity, shadow, light] : view.each()) {
    const auto& dir = light.m_direction;
    scene->Update<DirectionShadowComponent>(entity, [&dir, &camera](auto& node) {
      node.UpdateShadowInfo(dir, *camera);
      return true;
    });
  }
}

void
RenderLightDataJob::UpdateShadowMapAtlasPosition(Scene* scene) {
  auto& world = scene->GetWorld();
  auto view = world.view<DirectionShadowComponent>();

  auto upper_power_of_4 = [](unsigned int x) {
    unsigned int t = 0;
    while (pow(4, t) < x) {
      t++;
    }
    return t;
  };

  auto shadowCount = view.size();
  int gridCount = 1 << upper_power_of_4(shadowCount);

  for (int i = 0; i < shadowCount; i++) {
    int row = i / gridCount;
    int col = i % gridCount;

    float XOffset = static_cast<float>(row) / gridCount;
    float YOffset = static_cast<float>(col) / gridCount;
    auto& comp = view.get<DirectionShadowComponent>(view[i]);
    scene->Update<DirectionShadowComponent>(view[i], [&](auto& component) {
      component.m_viewport.x = XOffset;
      component.m_viewport.y = YOffset;
      component.m_viewport.z = 1.0 / gridCount;
      component.m_viewport.w = 1.0 / gridCount;
      return true;
    });
  }
}

}  // namespace Marbas::Job
