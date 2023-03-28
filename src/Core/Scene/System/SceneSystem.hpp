#pragma once
#include <entt/entt.hpp>

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
  UpdateEveryFrame(Scene* scene) {
    // update scene aabb
    CreateAsset(scene);

    UpdateAABBComponent(scene);
    UpdateTransformComp(scene);
    UploadGPUAsset(scene);
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
};

}  // namespace Marbas
