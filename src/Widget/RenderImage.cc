#include "Widget/RenderImage.hpp"

#include <ImGuizmo.h>

#include "Common.hpp"
#include "Core/Application.hpp"
#include "Core/Component.hpp"
#include "Core/Entity.hpp"
#include "Core/Window.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

void RenderImage::Draw() {
  auto window = Application::GetApplicationsWindow();
  auto renderLayer = window->GetRenderLayer();
  if (renderLayer == nullptr) return;

  /**
   * push the render result into the image
   */
  imageSize = ImGui::GetContentRegionAvail();
  auto textureId = const_cast<ImTextureID>(renderLayer->GetFrameBufferTexture());
  ImGui::Image(textureId, imageSize, ImVec2(0, 1), ImVec2(1, 0));

  /**
   *
   */

  if (m_scene == nullptr || !m_selectedMesh.has_value()) {
    /**
     * set view matrix for next render
     */
    auto camera = renderLayer->GetCamera();
    float aspect = imageSize.x / imageSize.y;
    camera->SetAspect(aspect);
    return;
  }

  // set imguizmo draw array
  auto winPos = ImGui::GetWindowPos();

  ImVec2 vMin = ImGui::GetWindowContentRegionMin();
  ImVec2 vMax = ImGui::GetWindowContentRegionMax();

  ImGuizmo::SetOrthographic(false);
  ImGuizmo::SetDrawlist();
  ImGuizmo::SetRect(winPos.x + vMin.x, winPos.y + vMin.y, imageSize.x, imageSize.y);

  // draw
  auto camera = renderLayer->GetCamera();
  auto viewMatrix = camera->GetViewMartix();
  auto perspectiveMatrix = camera->GetPerspective();

  auto& transform = Entity::GetComponent<TransformComponent>(m_scene, m_selectedMesh.value());

  ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix),
                       ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL,
                       glm::value_ptr(transform.modelMatrix));

  /**
   * set view matrix for next render
   */
  float aspect = imageSize.x / imageSize.y;
  camera->SetAspect(aspect);
}

}  // namespace Marbas
