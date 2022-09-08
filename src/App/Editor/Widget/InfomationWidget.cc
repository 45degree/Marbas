#include "App/Editor/Widget/InfomationWidget.hpp"

// clang-format off
#include <imgui.h>
#include <ImGuizmo.h>
// clang-format on

#include "Common/MathCommon.hpp"
#include "Core/Scene/Component/TagComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"

namespace Marbas {

void
InformationWidget::Draw() {
  if (m_entity == entt::null) return;
  if (!Entity::HasComponent<UniqueTagComponent>(m_scene.get(), m_entity)) return;

  const auto& tagComponent = Entity::GetComponent<UniqueTagComponent>(m_scene.get(), m_entity);
  switch (tagComponent.type) {
    case EntityType::Model:
      m_modelInformation->DrawInformation(m_entity, m_scene.get(), m_resourceManager.get());
      break;
    case EntityType::Light:
      m_lightInformation->DrawInformation(m_entity, m_scene.get(), m_resourceManager.get());
      break;
    default:
      break;
  }

  // TODO:
  // if (!m_selectedMesh.has_value()) return;
  //
  // auto mesh = m_selectedMesh.value();
  // auto& transform = Entity::GetComponent<TransformComponent>(m_scene, mesh);
  // auto& renderComponent = Entity::GetComponent<RenderComponent>(m_scene, mesh);
  // auto& modelmatrix = transform.modelMatrix;
  //
  // float matrixTranslation[3], matrixRotation[3], matrixScale[3];
  // ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelmatrix), matrixTranslation,
  //                                       matrixRotation, matrixScale);
  //
  // static char s8_v = 127;
  // if (ImGui::TreeNode("transform")) {
  //   ImGui::InputFloat3("translate", matrixTranslation);
  //   ImGui::InputFloat3("rotation", matrixRotation);
  //   ImGui::InputFloat3("scale", matrixScale);
  //
  //   ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale,
  //                                           glm::value_ptr(modelmatrix));
  //
  //   ImGui::TreePop();
  // }
  //
  // if (ImGui::TreeNode("Material")) {
  //   if (renderComponent.m_materialResource != nullptr) {
  //     const auto& resource = renderComponent.m_materialResource;
  //     auto* material = resource->GetMaterial();
  //     if (material != nullptr) {
  //       auto regionSize = ImGui::GetContentRegionAvail();
  //       if (ImGui::TreeNode("Diffuse Texture")) {
  //         auto* texture = material->GetDiffuseTexture();
  //         if (texture != nullptr) {
  //           auto textureId = reinterpret_cast<ImTextureID>(texture->GetTexture());
  //           ImGui::Image(textureId, ImVec2(regionSize.x, regionSize.x));
  //         }
  //         ImGui::TreePop();
  //       }
  //       if (ImGui::TreeNode("Ambient Texture")) {
  //         auto* texture = material->GetAmbientTexture();
  //         if (texture != nullptr) {
  //           auto textureId = reinterpret_cast<ImTextureID>(texture->GetTexture());
  //           ImGui::Image(textureId, ImVec2(regionSize.x, regionSize.x));
  //         }
  //         ImGui::TreePop();
  //       }
  //     }
  //   }
  //   ImGui::TreePop();
  // }
}

}  // namespace Marbas
