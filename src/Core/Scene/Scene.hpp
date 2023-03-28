#pragma once

#include <entt/entt.hpp>

#include "Common/Common.hpp"
#include "Common/EditorCamera.hpp"
#include "Core/Scene/Component/SceneNodeComponent.hpp"
#include "Core/Scene/Component/ShadowComponent.hpp"
#include "Core/Scene/Component/TransformComp.hpp"
#include "System/SceneSystem.hpp"

namespace Marbas {

class Scene {
 public:
  explicit Scene();
  explicit Scene(entt::registry&& registry);

 public:
  static std::shared_ptr<Scene>
  LoadFromFile(const Path& scenePath);

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
  GetRootNode() const {
    return m_rootEntity;
  }

  entt::entity
  AddChild(entt::entity parent);

  Vector<entt::entity>
  GetChildren(entt::entity node) const;

  entt::entity
  GetChild(entt::entity node, size_t index) const;

  size_t
  GetChildrenCount(entt::entity node) const;

  std::shared_ptr<EditorCamera>
  GetEditorCamrea() {
    return m_editorCamera;
  }

  const std::shared_ptr<EditorCamera>
  GetEditorCamera() const {
    return m_editorCamera;
  }

  void
  Update() {
    // LightSceneNode::Update(m_world);
    // ModelSceneNode::Update(m_world);
    // DirectionShadowComponent::Update(m_world, m_editorCamera.get());
    // RenderMeshComponent::Update(m_world, resourceManager);
    SceneSystem::UpdateEveryFrame(this);
  }

 private:
  String m_name = "default scene";
  entt::registry m_world;
  entt::entity m_rootEntity;
  std::shared_ptr<EditorCamera> m_editorCamera = nullptr;
};

}  // namespace Marbas
