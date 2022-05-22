#pragma once

#include <entt/entt.hpp>

#include "Resource/ModelResource.hpp"

namespace Marbas {

/**
 * the model entity is the entity to descripte the model, it just contain the base information
 * of the model, such as the position, rotate etc.
 *
 * the model entity itself don't contain the meshes, it is defined in the mesh entity, so the model
 * entity must have an hierarchy component to indicate the first mesh entity.
 */

using ModelEntity = entt::entity;

class ModelEntityPolicy {
 public:
  /**
   * @berif create the model entity from the model resource
   *
   * @note it will also create the mesh entity and regist it to the registry
   */
  static ModelEntity
  Create(entt::registry& registry, Uid modelResourceId,
         const std::shared_ptr<ResourceManager>& resourceManager);
};

}  // namespace Marbas
