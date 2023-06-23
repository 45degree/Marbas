#include "RenderViewClipJob.hpp"

#include "Core/Scene/System/RenderSystemJob/RenderSystem.hpp"

namespace Marbas::Job {

void
RenderViewClipJob::update(DeltaTime deltaTime, void* data) {
  auto* renderInfo = reinterpret_cast<RenderInfo*>(data);
  auto* scene = renderInfo->scene;
  auto& world = scene->GetWorld();

  auto camera = scene->GetEditorCamrea();
  auto& frustum = camera->GetFrustum();

  auto modelView = world.view<ModelSceneNode, AABBComponent, TransformComp>();

  for (auto&& [entity, node, aabb, tran] : modelView.each()) {
    if (!aabb.IsOnFrustum(frustum, tran.GetGlobalTransform())) {
      if (world.any_of<RenderableTag>(entity)) {
        scene->Remove<RenderableTag>(entity);
      }
    } else {
      if (!world.any_of<RenderableTag>(entity)) {
        scene->Emplace<RenderableTag>(entity);
      }
    }
  }

  return;
}

}  // namespace Marbas::Job
