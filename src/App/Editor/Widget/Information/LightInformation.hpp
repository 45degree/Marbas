#pragma once

#include <entt/entt.hpp>

#include "Core/Scene/Scene.hpp"

namespace Marbas {

class LightInformation {
 public:
  void
  DrawInformation(entt::entity entity, Scene* scene, ResourceManager* resourceManager);
};

}  // namespace Marbas
