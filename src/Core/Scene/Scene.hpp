#pragma once

#include <toml++/toml.h>

#include <entt/entt.hpp>

#include "Common/Common.hpp"
#include "Common/EditorCamera.hpp"
#include "Core/Scene/Entity/LightEntity.hpp"
#include "Resource/ResourceManager.hpp"

namespace Marbas {

class Scene : public std::enable_shared_from_this<Scene> {
  friend class Entity;

 public:
  explicit Scene(const std::shared_ptr<ResourceManager>& resourceManager);

 public:
  void
  ReadFromFile(const Path& scenePath);

  void
  ReadFromString(const String& sceneStr);

  void
  SaveToFile(const Path& scenePath);

  String
  GetSceneName() const noexcept {
    return m_name;
  }

  const entt::registry&
  GetWorld() const {
    return m_world;
  }

  entt::registry&
  GetWorld() {
    return m_world;
  }

  entt::entity
  GetRootEntity() const {
    return m_rootEntity;
  }

  void
  AddModel(Uid modelResourceId, const String& modelName, const entt::entity& parent);

  void
  AddBillBoard(Uid texture2DResourceId, glm::vec3 point, const entt::entity& parent);

  void
  AddEmptyNode(const entt::entity& parent);

  void
  AddLight(LightType type, glm::vec3 point, const entt::entity& parent);

  std::shared_ptr<EditorCamera>
  GetEditorCamrea() {
    return m_editorCamera;
  }

  const std::shared_ptr<EditorCamera>
  GetEditorCamera() const {
    return m_editorCamera;
  }

 private:
  void
  ProcessScene(const toml::table& sceneTomlTable);

 private:
  String m_name = "default scene";
  entt::registry m_world;
  entt::entity m_rootEntity;
  std::shared_ptr<ResourceManager> m_resourceManager = nullptr;
  std::shared_ptr<EditorCamera> m_editorCamera = nullptr;
};

}  // namespace Marbas
