#pragma once

#include <glog/logging.h>

#include <entt/entt.hpp>
#include <exception>

#include "Core/Scene/Scene.hpp"

namespace Marbas {

class Entity {
 public:
  Entity() = delete;
  Entity(const Entity&) = delete;
  Entity&
  operator=(const Entity&) = delete;

  template <typename Policy, typename... Args>
  static entt::entity
  CreateEntity(Scene* scene, Args&&... args) {
    auto res = Policy::Create(scene->m_world, std::forward<Args>(args)...);
    return res;
  }

  template <typename Component>
  static bool
  HasComponent(const Scene* scene, const entt::entity& entityHandle) {
    return scene->m_world.any_of<Component>(entityHandle);
  }

  template <typename T, typename... Args>
  static void
  AddComponent(Scene* scene, const entt::entity& entityHandle, Args&&... args) {
    if (HasComponent<T>(scene, entityHandle)) {
      LOG(INFO) << FORMAT("this eneity has {}", typeid(T).name());
      return;
    }

    scene->m_world.emplace<T>(entityHandle, std::forward<Args>(args)...);
  }

  template <typename T>
  static const T&
  GetComponent(const Scene* scene, const entt::entity& entityHandle) {
    if (!HasComponent<T>(scene, entityHandle)) {
      String errorMsg = FORMAT("this entity has {}", typeid(T).name());
      LOG(INFO) << errorMsg;
      throw std::runtime_error(errorMsg.c_str());
    }

    return scene->m_world.get<T>(entityHandle);
  }

  template <typename T>
  static T&
  GetComponent(Scene* scene, const entt::entity& entityHandle) {
    if (!HasComponent<T>(scene, entityHandle)) {
      String errorMsg = FORMAT("this entity has {}", typeid(T).name());
      LOG(INFO) << errorMsg;
      throw std::runtime_error(errorMsg.c_str());
    }

    return scene->m_world.get<T>(entityHandle);
  }

  template <typename T>
  static void
  RemoveComponent(Scene* scene, entt::entity& entityHandle) {
    if (!HasComponent<T>(scene, entityHandle)) return;

    scene->m_world.remove<T>(entityHandle);
  }

  template <typename... Components>
  static const auto
  GetAllEntity(const Scene* scene) {
    return scene->m_world.view<Components...>();
  }

  template <typename... Components>
  static auto
  GetAllEntity(Scene* scene) {
    return scene->m_world.view<Components...>();
  }
};

}  // namespace Marbas
