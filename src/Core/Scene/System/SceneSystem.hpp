#pragma once
#include <entt/entt.hpp>

#include "AssetManager/AssetManager.hpp"
#include "Core/Scene/System/SceneSystemJob/SceneSystem.hpp"
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
  Initialize();

  static Task<void>
  Update(Scene* scene);

 private:
  /**
   * @brief create the asset that don't have a uid(don't have a cache in disk);
   *
   * @param scene scene
   */
  static Task<void>
  CreateAssetCache(Scene* scene);

  static Job::SceneSystem s_sceneSystem;
};

}  // namespace Marbas
