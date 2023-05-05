#pragma once
#include <entt/entt.hpp>

#include "AssetManager/AssetManager.hpp"
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
  static Task<void>
  Update(Scene* scene) {
    co_await CreateAssetCache(scene);
    co_await LoadMesh(scene);
    ClearUnuseAsset(scene);

    // update scene aabb
    UpdateAABBComponent(scene);
    UpdateTransformComp(scene);

    co_return;
  }

 private:
  /**
   * @brief create the asset that don't have a uid(don't have a cache in disk);
   *
   * @param scene scene
   */
  static Task<void>
  CreateAssetCache(Scene* scene);

  static Task<void>
  LoadMesh(Scene* scene);

  static void
  ClearUnuseAsset(Scene* scene);

  static void
  UpdateTransformComp(Scene* scene);

  static void
  UpdateTransformCompForEntity(Scene* scene, entt::entity entity);

  static void
  UpdateAABBComponent(Scene* scene);
};

}  // namespace Marbas
