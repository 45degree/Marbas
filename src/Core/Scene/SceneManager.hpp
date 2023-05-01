#pragma once

#include <algorithm>

#include "AssetManager/AssetManager.hpp"
#include "AssetManager/AssetPath.hpp"
#include "AssetManager/Singleton.hpp"
#include "Scene.hpp"

namespace Marbas {

class SceneManagerBase {
 public:
  Scene*
  GetScene(const AssetPath& path) {
    if (m_namedScene.find(path) == m_namedScene.end()) return nullptr;
    return m_namedScene.at(path).get();
  };

  void
  LoadScene(const AssetPath& path);

  void
  SaveScene(const AssetPath& path, Scene* scene);

  Scene*
  CreateEmptyScene();

  void
  SetActiveScene(Scene* scene) {
    m_activeScene = scene;
  }

  void
  SetActiveScene(const AssetPath& path) {
    if (m_namedScene.find(path) == m_namedScene.end()) {
      LoadScene(path);
    }

    m_activeScene = m_namedScene.at(path).get();
  }

  bool
  IsUnNamedScene(const Scene* scene) const {
    return std::any_of(m_unnamedScene.begin(), m_unnamedScene.end(), [&](auto& unnameScene) {
      const auto* _unnameScene = unnameScene.get();
      return _unnameScene == scene;
    });
  }

  void
  DeleteScene(const Scene* scene);

  Scene*
  GetActiveScene() {
    return m_activeScene;
  }

  bool
  IsSceneNeedSave(Scene* scene);

 private:
  void
  SetPathForUnnameScene(const AssetPath& path, const Scene* scene) {
    auto iter = std::find_if(m_unnamedScene.begin(), m_unnamedScene.end(), [&](auto& unnameScene) {
      const auto* _unnameScene = unnameScene.get();
      return _unnameScene == scene;
    });
    if (iter == m_unnamedScene.end()) return;

    auto unnameScene = std::move(*iter);
    m_namedScene.insert_or_assign(path, std::move(unnameScene));
    m_unnamedScene.erase(iter);
  }

 private:
  std::unordered_map<AssetPath, std::unique_ptr<Scene>> m_namedScene;
  std::vector<std::unique_ptr<Scene>> m_unnamedScene;

  Scene* m_activeScene = nullptr;
};

using SceneManager = Singleton<SceneManagerBase>;

}  // namespace Marbas
