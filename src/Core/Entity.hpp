#ifndef MARBAS_CORE_ENTITY_HPP
#define MARBAS_CORE_ENTITY_HPP

#include <glog/logging.h>

#include <entt/entt.hpp>
#include <exception>

#include "Core/Scene/Scene.hpp"

namespace Marbas {

class Entity {
 public:
  Entity() = delete;
  Entity(const Entity&) = delete;
  Entity& operator=(const Entity&) = delete;

  template <typename Policy>
  static entt::entity CreateEntity(Scene* scene) {
    return Policy::Create(scene->m_registry);
  }

  template <typename Component>
  static bool HasComponent(const Scene* scene, const entt::entity& entityHandle) {
    return scene->m_registry.any_of<Component>(entityHandle);
  }

  template <typename T, typename... Args>
  static void AddComponent(Scene* scene, const entt::entity& entityHandle, Args&&... args) {
    if (HasComponent<T>(scene, entityHandle)) {
      LOG(INFO) << FORMAT("this eneity has {}", typeid(T).name());
      return;
    }

    scene->m_registry.emplace<T>(entityHandle, std::forward<Args>(args)...);
  }

  template <typename T>
  static T& GetComponent(Scene* scene, const entt::entity& entityHandle) {
    if (!HasComponent<T>(scene, entityHandle)) {
      String errorMsg = FORMAT("this entity has {}", typeid(T).name());
      LOG(INFO) << errorMsg;
      throw std::runtime_error(errorMsg.c_str());
    }

    return scene->m_registry.get<T>(entityHandle);
  }

  template <typename T>
  static void RemoveComponent(Scene* scene, entt::entity& entityHandle) {
    if (!HasComponent<T>(scene, entityHandle)) return;

    scene->m_registry.remove<T>(entityHandle);
  }
};

}  // namespace Marbas

#endif
