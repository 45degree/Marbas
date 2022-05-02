#include "Widget/MeshInfomationWidget.hpp"

#include <ImGuizmo.h>
#include <imgui.h>

#include "Core/Component.hpp"
#include "Core/Entity.hpp"
#include "MathCommon.hpp"

namespace Marbas {

void MeshInfomationWidget::Draw() {
  if (!m_selectedMesh.has_value()) return;

  auto mesh = m_selectedMesh.value();
  auto& transform = Entity::GetComponent<TransformComponent>(m_scene, mesh);
  auto& renderComponent = Entity::GetComponent<RenderComponent>(m_scene, mesh);
  auto& modelmatrix = transform.modelMatrix;

  float matrixTranslation[3], matrixRotation[3], matrixScale[3];
  ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelmatrix), matrixTranslation,
                                        matrixRotation, matrixScale);

  static char s8_v = 127;
  if (ImGui::TreeNode("transform")) {
    ImGui::InputFloat3("translate", matrixTranslation);
    ImGui::InputFloat3("rotation", matrixRotation);
    ImGui::InputFloat3("scale", matrixScale);

    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale,
                                            glm::value_ptr(modelmatrix));

    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Material")) {
    if (renderComponent.m_materialResource.has_value()) {
      auto materialResourceUid = renderComponent.m_materialResource.value();
      auto* resource = m_resourceManager->FindMaterialResource(materialResourceUid);
      auto* material = resource->GetMaterial();
      if (material != nullptr) {
        auto regionSize = ImGui::GetContentRegionAvail();
        if (ImGui::TreeNode("Diffuse Texture")) {
          auto* texture = material->GetDiffuseTexture();
          if (texture != nullptr) {
            auto textureId = reinterpret_cast<ImTextureID>(texture->GetTexture());
            ImGui::Image(textureId, ImVec2(regionSize.x, regionSize.x));
          }
          ImGui::TreePop();
        }
        if (ImGui::TreeNode("Ambient Texture")) {
          auto* texture = material->GetAmbientTexture();
          if (texture != nullptr) {
            auto textureId = reinterpret_cast<ImTextureID>(texture->GetTexture());
            ImGui::Image(textureId, ImVec2(regionSize.x, regionSize.x));
          }
          ImGui::TreePop();
        }
      }
    }
    ImGui::TreePop();
  }
}

}  // namespace Marbas
