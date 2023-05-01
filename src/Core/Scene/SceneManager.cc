#include "SceneManager.hpp"

#include <glog/logging.h>

#include "AssetManager/AssetRegistry.hpp"

namespace Marbas {

void
SceneManagerBase::LoadScene(const AssetPath& path) {
  auto scenePath = path.GetAbsolutePath();
  auto scene = Scene::LoadFromFile(scenePath);

  m_namedScene.insert_or_assign(path, std::move(scene));
}

Scene*
SceneManagerBase::CreateEmptyScene() {
  m_unnamedScene.push_back(std::make_unique<Scene>());
  return m_unnamedScene.back().get();
}

void
SceneManagerBase::SaveScene(const AssetPath& path, Scene* scene) {
  scene->SaveToFile(path.GetAbsolutePath());
}

void
SceneManagerBase::DeleteScene(const Scene* scene) {
  // find in unnameScene
  {
    auto iter = std::find_if(m_unnamedScene.begin(), m_unnamedScene.end(), [&](auto& unnameScene) {
      const auto* _unnameScene = unnameScene.get();
      return _unnameScene == scene;
    });

    if (iter != m_unnamedScene.end()) {
      m_unnamedScene.erase(iter);
      return;
    }
  }

  // find in named scene
  {
    auto iter = std::find_if(m_namedScene.begin(), m_namedScene.end(), [&](auto& keyValue) {
      auto& [key, value] = keyValue;
      return value.get() == scene;
    });

    if (iter != m_namedScene.end()) {
      m_namedScene.erase(iter);
      return;
    }
  }

  return;
}

bool
SceneManagerBase::IsSceneNeedSave(Scene* scene) {
  if (IsUnNamedScene(scene)) return true;

  return false;
}

}  // namespace Marbas
