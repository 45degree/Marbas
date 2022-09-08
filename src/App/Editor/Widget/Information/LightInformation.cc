#include "App/Editor/Widget/Information/LightInformation.hpp"

#include <imgui.h>

#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"

namespace Marbas {
void
LightInformation::DrawInformation(entt::entity entity, Scene *scene,
                                  ResourceManager *resourceManager) {
  if (Entity::HasComponent<PointLightComponent>(scene, entity)) {
    // TODO: show point light info
    auto &pointLight = Entity::GetComponent<PointLightComponent>(scene, entity).m_light;
    auto pos = pointLight.GetPos();
    ImGui::Text("type: Point Light");
    ImGui::InputFloat3("location", glm::value_ptr(pos));
    pointLight.SetPos(pos);
  } else if (Entity::HasComponent<ParallelLightComponent>(scene, entity)) {
  }
}

}  // namespace Marbas
