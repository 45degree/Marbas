// #include "LightInformation.hpp"
//
// #include <imgui.h>
//
// #include "Core/Scene/Component/HierarchyComponent.hpp"
// #include "Core/Scene/Component/LightComponent.hpp"
//
// namespace Marbas {
//
// void
// LightInformation::DrawInformation(entt::entity entity, Scene* scene, ResourceManager* resourceManager) {
//   auto& world = scene->GetWorld();
//   auto& hierarchyComponent = world.get<HierarchyComponent>(entity);
//   auto& transformComponent = world.get<TransformComp>(entity);
//   auto& lightSceneNode = world.get<DirectionLightComponent>(entity);
//   auto& light = lightSceneNode.m_light;
//
//   glm::mat4 transformMatrix = transformComponent.GetGlobalTransform();
//   auto pos = glm::vec3(glm::column(transformMatrix, 3));
//
//   auto direction = light.GetDirection();
//   auto color = light.GetColor();
//   ImGui::Text("type: Direction Light");
//   ImGui::InputFloat3("location", glm::value_ptr(pos));
//   ImGui::InputFloat3("direction", glm::value_ptr(direction));
//   ImGui::ColorEdit3("color", glm::value_ptr(color));
//   transformMatrix[3] = glm::vec4(pos, 1);
//   light.SetDirection(direction);
//   light.SetColor(color);
// }
//
// // void
// // test() {
// //   if (world.any_of<PointLightComponent>(light)) {
// //     auto& pointLight = world.get<PointLightComponent>(light).m_light;
// //     auto color = pointLight.GetColor();
// //     ImGui::Text("type: Point Light");
// //     ImGui::InputFloat3("location", glm::value_ptr(pos));
// //     ImGui::ColorEdit3("color", glm::value_ptr(color));
// //     hierarchyComponent.globalTransformMatrix[3] = glm::vec4(pos, 1);
// //     pointLight.SetColor(color);
// //   } else if (world.any_of<DirectionLightComponent>(light)) {
// //   }
// // }
//
// }  // namespace Marbas
