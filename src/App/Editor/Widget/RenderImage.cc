#include "App/Editor/Widget/RenderImage.hpp"

#include <ImGuizmo.h>

#include "Common/Common.hpp"
#include "Core/Application.hpp"
// #include "Core/Scene/Component/Component.hpp"
#include "Core/Scene/Entity/Entity.hpp"
#include "Core/Window.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

void
RenderImage::Draw() {
  auto window = Application::GetApplicationsWindow();
  auto renderLayer = window->GetRenderLayer();
  DLOG_ASSERT(renderLayer != nullptr) << "render layer is nullptr";

  auto texture = renderLayer->GetRenderResult();

  /**
   * push the render result into the image
   */
  imageSize = ImGui::GetContentRegionAvail();
  auto textureId = const_cast<ImTextureID>(texture->GetTexture());
  ImGui::Image(textureId, imageSize, ImVec2(0, 1), ImVec2(1, 0));

  /**
   *
   */

  // set imguizmo draw array
  auto winPos = ImGui::GetWindowPos();
  auto camera = m_scene->GetEditorCamrea();

  ImVec2 vMin = ImGui::GetWindowContentRegionMin();
  ImVec2 vMax = ImGui::GetWindowContentRegionMax();

  ImGuizmo::SetOrthographic(false);
  ImGuizmo::SetDrawlist();
  ImGuizmo::SetRect(winPos.x + vMin.x, winPos.y + vMin.y, imageSize.x, imageSize.y);

  // draw
  // TODO:
  if (m_modelEntity.has_value()) {
    auto entity = m_modelEntity.value();
    auto viewMatrix = camera->GetViewMartix();
    auto perspectiveMatrix = camera->GetPerspective();
    const auto& modelComponent = Entity::GetComponent<ModelComponent>(m_scene, entity);
    auto id = modelComponent.modelResourceId;
    auto resource = m_resourceManager->GetModelResourceContainer()->GetResource(id);
    DLOG_ASSERT(resource != nullptr);

    auto modelMatrix = resource->GetModel()->GetModelMatrix();

    // ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix), matrixTranslation,
    //                                       matrixRotation, matrixScale);

    // ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale,
    //                                         glm::value_ptr(modelMatrix));
    //
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix),
                         ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL,
                         glm::value_ptr(modelMatrix));

    resource->GetModel()->SetModelMatrix(modelMatrix);
  }

  /**
   * set view matrix for next render
   */
  float aspect = imageSize.x / imageSize.y;
  camera->SetAspect(aspect);
}

}  // namespace Marbas
