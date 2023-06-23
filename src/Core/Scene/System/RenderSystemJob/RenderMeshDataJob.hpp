#pragma once

#include <entt/entt.hpp>

#include "AssetManager/Mesh.hpp"
#include "Core/Scene/Component/RenderComponent/MeshRenderComponent.hpp"

namespace Marbas::Job {

/**
 * @class RenderMeshDataJob
 * @brief prepare the mesh data which need to be rendered
 *
 */
class RenderMeshDataJob : public entt::process<RenderMeshDataJob, uint32_t> {
  using DeltaTime = uint32_t;

 public:
  RenderMeshDataJob(RHIFactory* rhiFactory);
  ~RenderMeshDataJob() override;

 public:
  void
  init();

  void
  update(DeltaTime deltaTime, void* data);

 private:
  RHIFactory* m_rhiFactory;
  uintptr_t m_sampler;
  Image* m_emptyImage;
  ImageView* m_emptyImageView;
};

};  // namespace Marbas::Job
