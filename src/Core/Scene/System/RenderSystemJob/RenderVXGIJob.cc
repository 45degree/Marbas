#include "RenderVXGIJob.hpp"

#include "Core/Scene/Component/Component.hpp"
#include "Core/Scene/Component/SerializeComponent/TransformComp.hpp"
#include "Core/Scene/System/RenderSystemJob/RenderSystem.hpp"

namespace Marbas::Job {

RenderVXGIJob::RenderVXGIJob(RHIFactory* rhiFactory) : m_rhiFactory(rhiFactory), m_activeProbe({entt::null}) {}

RenderVXGIJob::~RenderVXGIJob() {}

void
RenderVXGIJob::update(DeltaTime deltaTime, void* data) {
  auto* renderInfo = reinterpret_cast<RenderInfo*>(data);
  auto* userData = renderInfo->userData;
  auto* scene = reinterpret_cast<Job::RenderUserData*>(userData)->scene;
  auto& world = scene->GetWorld();
  auto rootEntity = scene->GetRootNode();

  auto camera = scene->GetEditorCamrea();
  auto& frustum = camera->GetFrustum();

  // fina all visible vxgi probe and create the render info
  auto sceneGIProbeView = world.view<VXGIProbeSceneNode, TransformComp>();
  for (auto entity : sceneGIProbeView) {
    auto& sceneGIProbe = world.get<VXGIProbeSceneNode>(entity);
    auto& transform = world.get<TransformComp>(entity);
    auto& globalTransform = transform.GetGlobalTransform();
    auto pos = glm::vec3(globalTransform[3]);

    AABBComponent _tempCom(pos - sceneGIProbe.size / 2.f, pos + sceneGIProbe.size / 2.f);
    if (_tempCom.IsOnFrustum(frustum, transform.GetGlobalTransform())) {
      if (!world.any_of<VoxelRenderComponent>(entity)) {
        scene->Emplace<VoxelRenderComponent>(entity, m_rhiFactory, sceneGIProbe.size);
      }
      scene->Update<VoxelRenderComponent>(entity, [&](auto& component) {
        component.UpdateVoxelInfo(sceneGIProbe.size, pos);
        return true;
      });
    } else {
      // TODO: maybe need a cache
      scene->Remove<VoxelRenderComponent>(entity);
    }
  }

  // create global vxgi component
  if (!world.any_of<VXGIGlobalComponent>(rootEntity)) {
    if (sceneGIProbeView.size_hint() == 0) {
      return;
    }
    scene->Emplace<VXGIGlobalComponent>(rootEntity, m_rhiFactory);
  }

  scene->Update<VXGIGlobalComponent>(rootEntity, [&](auto& component) {
    // find all unactive probe in last scene
    std::array<size_t, VXGIGlobalComponent::maxProbeCount> unactiveList;
    int index = 0;

    for (int i = 0; i < m_activeProbe.size(); i++) {
      auto& entity = m_activeProbe[i];
      if (entity == entt::null) {
        unactiveList[index++] = i;
        continue;
      }
      if (!world.any_of<VoxelRenderComponent>(entity)) {
        m_activeProbe[i] = entt::null;
        unactiveList[index++] = i;
        continue;
      }
      component.UpdateVoxelProbe(world.get<VoxelRenderComponent>(entity), i);
    }

    // find current active probe
    auto activeGIProbeView = world.view<VoxelRenderComponent>();
    for (int i = 0, j = 0; i < activeGIProbeView.size() && j < index; i++) {
      auto entity = activeGIProbeView[i];
      if (std::find(m_activeProbe.begin(), m_activeProbe.end(), entity) == m_activeProbe.end()) {
        m_activeProbe[unactiveList[j]] = entity;
        component.BindVoxelProbe(world.get<VoxelRenderComponent>(entity), unactiveList[j]);
      }
      j++;
    }
    return true;
  });
}

}  // namespace Marbas::Job
