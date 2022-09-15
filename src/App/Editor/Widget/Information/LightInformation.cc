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
    auto color = pointLight.GetColor();
    ImGui::Text("type: Point Light");
    ImGui::InputFloat3("location", glm::value_ptr(pos));
    ImGui::InputFloat3("color", glm::value_ptr(color));
    pointLight.SetPos(pos);
    pointLight.SetColor(color);
  } else if (Entity::HasComponent<ParallelLightComponent>(scene, entity)) {
    auto &directionLight = Entity::GetComponent<ParallelLightComponent>(scene, entity).m_light;
    auto pos = directionLight.GetPos();
    auto direction = directionLight.GetDirection();
    ImGui::Text("type: Direction Light");
    ImGui::InputFloat3("location", glm::value_ptr(pos));
    ImGui::InputFloat3("direction", glm::value_ptr(direction));
    directionLight.SetPos(pos);
    directionLight.SetDirection(direction);
  }
}

}  // namespace Marbas
