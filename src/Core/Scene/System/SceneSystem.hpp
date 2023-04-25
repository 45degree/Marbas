#pragma once
#include <entt/entt.hpp>

#include "RHIFactory.hpp"

namespace Marbas {

class Scene;

/**
 * @class SceneSystem
 * @brief this system used to handle all scene node
 *
 */
struct SceneSystem {
 public:
  static void
  UpdateEveryFrame(Scene* scene, RHIFactory* rhiFactory) {
    // update scene aabb
    CreateAsset(scene);

    UpdateAABBComponent(scene);
    UpdateTransformComp(scene);
    UpdateDirectionShadowInfo(scene);
    UploadGPUAsset(scene);

    UpdateMeshGPUAsset(scene, rhiFactory);
    UpdateLightGPUData(scene);
  }

 private:
  static void
  CreateAsset(Scene* scene);

  static void
  UploadGPUAsset(Scene* scene);

  static void
  UpdateTransformComp(Scene* scene);

  static void
  UpdateTransformCompForEntity(Scene* scene, entt::entity entity);

  static void
  UpdateAABBComponent(Scene* scene);

  static void
  UpdateDirectionShadowInfo(Scene* scene);

  /**
   * @brief update mesh gpu asset, such as material and so on;
   *
   * @param scene scene
   * @param rhiFactory rhi factory
   */
  static void
  UpdateMeshGPUAsset(Scene* scene, RHIFactory* rhiFactory);

  static void
  UpdateLightGPUData(Scene* scene);
};

}  // namespace Marbas
