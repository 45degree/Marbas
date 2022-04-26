#ifndef MARBAS_CORE_ENTITY_HPP
#define MARBAS_CORE_ENTITY_HPP

#include "Core/Scene.hpp"

#include <glog/logging.h>
#include <entt/entt.hpp>
#include <exception>

namespace Marbas {

class Entity {
public:
    Entity() = delete;
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    template<typename Policy>
    static entt::entity CreateEntity(Scene* scene) {
        return Policy::Create(scene->m_registry);
    }

    template<typename Component>
    static bool HasComponent(const Scene* scene, const entt::entity& entityHandle) {
        return scene->m_registry.any_of<Component>(entityHandle);
    }

    template<typename T, typename... Args>
    static void AddComponent(Scene* scene, const entt::entity& entityHandle, Args&&... args) {
        if(HasComponent<T>()) {
            LOG(INFO) << FORMAT("this eneity has {}", typeid(T).name());
            return;
        }

        scene->m_registry.emplace<T>(entityHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    static T& GetComponent(const Scene* scene, const entt::entity& entityHandle) {
        if(!HasComponent<T>()) {
            String errorMsg = FORMAT("this entity has {}", typeid(T).name());
            LOG(INFO) << errorMsg;
            throw std::runtime_error(errorMsg);
        }

        return scene->m_registry.get<T>(entityHandle);
    }

    template<typename T>
    static void RemoveComponent(Scene* scene, const entt::entity& entityHandle) {
        if(!HasComponent<T>()) return;

        scene->m_registry.remove<T>(entityHandle);
    }
};

}  // namespace Marbas

#endif
