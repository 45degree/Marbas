#pragma once

#include <entt/entity/fwd.hpp>
#include <entt/entity/observer.hpp>
#include <entt/entt.hpp>

#include "Common/Common.hpp"
#include "Common/EditorCamera.hpp"
#include "Component/Component.hpp"

namespace Marbas {

template <typename F, typename Component>
concept ComponentUpdateFunc = requires(F&& f, Component component) {
  { f(component) } -> std::same_as<bool>;
};

class Scene {
 public:
  explicit Scene();
  explicit Scene(entt::registry&& registry);

 public:
  static std::unique_ptr<Scene>
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

  entt::entity
  CreateEntity() {
    return m_world.create();
  }

  entt::entity
  CreateSceneNode() {
    throw std::logic_error("not implement");
  };

  template <typename Type, typename... Other, typename... Exclude>
  [[nodiscard]] decltype(auto)
  View(entt::exclude_t<Exclude...> exclude = entt::exclude_t<>{}) const {
    return m_world.view<Type, Other...>(exclude);
  }

  template <typename Component>
  const Component&
  Get(entt::entity entity) {
    return m_world.get<Component>(entity);
  }

  template <typename Component, typename... Args>
  void
  Emplace(Args&&... args) {
    m_world.emplace<Component>(std::forward<Args>(args)...);
  }

  template <typename Component, ComponentUpdateFunc<Component> Func>
  void
  Update(const entt::entity entity, Func&& func) {
    auto& component = m_world.get<Component>(entity);
    if (func(component)) {
      m_world.patch<Component>(entity);
    }
  }

  template <typename Component>
  void
  Remove(const entt::entity entity) {
    m_world.remove<Component>(entity);
  }

  template <typename... Components>
  bool
  AnyOf(const entt::entity& entity) const {
    return m_world.any_of<Components...>(entity);
  }

  template <typename Collector>
  void
  ConnectObserve(entt::observer& observer, Collector&& collector) {
    observer.connect(m_world, std::forward<Collector>(collector));
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
  GetEditorCamrea() const {
    return m_editorCamera;
  }

  const std::shared_ptr<EditorCamera>
  GetEditorCamera() const {
    return m_editorCamera;
  }

 private:
  String m_name = "default scene";
  entt::registry m_world;
  entt::entity m_rootEntity = entt::null;
  std::shared_ptr<EditorCamera> m_editorCamera = nullptr;
};

}  // namespace Marbas
