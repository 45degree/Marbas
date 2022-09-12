#include "App/Editor/Widget/RenderImage.hpp"

#include <IconsFontAwesome6.h>
#include <ImGuizmo.h>

#include "Common/Common.hpp"
#include "Core/Application.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"
#include "Core/Window.hpp"
#include "RHI/RHI.hpp"
#include "imgui.h"
#include "imgui_internal.h"

namespace Marbas {

void
RenderImage::ShowToolBar() {
  ImGui::PushID(0);
  ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f, 0.));

  if (ImGui::Button(ICON_FA_ARROW_POINTER)) {
    m_showMove = true;
    m_showRotate = true;
    m_showScale = true;
  }

  ImGui::SameLine();
  ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT)) {
    m_showMove = true;
    m_showRotate = false;
    m_showScale = false;
  }

  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_ARROW_ROTATE_LEFT)) {
    m_showMove = false;
    m_showRotate = true;
    m_showScale = false;
  }

  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_MAXIMIZE)) {
    m_showMove = false;
    m_showRotate = false;
    m_showScale = true;
  }

  ImGui::SameLine();
  ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_ARROWS_TO_DOT)) {
    if (m_entity.has_value()) {
      const auto& tagComp = Entity::GetComponent<UniqueTagComponent>(m_scene.get(), *m_entity);
      auto type = tagComp.type;
      if (type == EntityType::Model) {
        auto entity = *m_entity;
        auto camera = m_scene->GetEditorCamrea();
        auto perspectiveMatrix = camera->GetProjectionMatrix();
        const auto& modelComponent = Entity::GetComponent<ModelComponent>(m_scene.get(), entity);
        auto id = modelComponent.modelResourceId;
        auto resource = m_resourceManager->GetModelResourceContainer()->GetResource(id);
        DLOG_ASSERT(resource != nullptr);

        auto modelMatrix = resource->GetModel()->GetModelMatrix();

        float modelPos[3], unused1[3], unused2[3];
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix), modelPos, unused1,
                                              unused2);
        auto cameraPos = camera->GetPosition();
        camera->MovePosition(glm::vec3(modelPos[0], modelPos[1], modelPos[2]));
      }
    }
  }

  ImGui::PopStyleColor(1);
  ImGui::PopID();
}

void
RenderImage::Draw() {
  auto window = Application::GetApplicationsWindow();
  auto renderLayer = window->GetRenderLayer();
  DLOG_ASSERT(renderLayer != nullptr) << "render layer is nullptr";

  ShowToolBar();

  auto texture = renderLayer->GetRenderResult();

  /**
   * push the render result into the image
   */
  imageSize = ImGui::GetContentRegionAvail();
  auto textureId = const_cast<ImTextureID>(texture->GetOriginHandle());
  auto ImagePos = ImGui::GetCursorPos();
  ImGui::Image(textureId, imageSize, ImVec2(0, 1), ImVec2(1, 0));

  /**
   *
   */

  // set imguizmo draw array
  auto winPos = ImGui::GetWindowPos();
  auto camera = m_scene->GetEditorCamrea();

  ImVec2 vMax = ImGui::GetWindowContentRegionMax();

  ImGuizmo::SetOrthographic(false);
  ImGuizmo::SetDrawlist();
  ImGuizmo::SetRect(winPos.x + ImagePos.x, winPos.y + ImagePos.y, vMax.x, vMax.y);

  auto viewMatrix = camera->GetViewMatrix();
  auto ViewManipulateSize = ImVec2(100, 100);
  auto ViewManipulatePos = ImVec2(winPos.x + vMax.x - ViewManipulateSize.x, winPos.y + ImagePos.y);
  ImGuizmo::ViewManipulate(glm::value_ptr(viewMatrix), camera->GetDistance(), ViewManipulatePos,
                           ViewManipulateSize, 0x4FFFFFFF);
  camera->SetViewMatrix(viewMatrix);

  // draw
  // TODO:
  if (m_entity.has_value()) {
    const auto& tagComp = Entity::GetComponent<UniqueTagComponent>(m_scene.get(), *m_entity);
    auto type = tagComp.type;
    switch (type) {
      case EntityType::Model:
        DrawModelManipulate();
        break;
      case EntityType::Light:
        DrawLightManipulate();
        break;
      default:
        break;
    }
  }

  /**
   * set view matrix for next render
   */
  float aspect = imageSize.x / imageSize.y;
  camera->SetAspect(aspect);
}

void
RenderImage::DrawModelManipulate() {
  const auto camera = m_scene->GetEditorCamrea();
  const auto viewMatrix = camera->GetViewMatrix();
  const auto perspectiveMatrix = camera->GetProjectionMatrix();

  auto entity = m_entity.value();
  const auto& modelComponent = Entity::GetComponent<ModelComponent>(m_scene.get(), entity);
  auto id = modelComponent.modelResourceId;
  auto resource = m_resourceManager->GetModelResourceContainer()->GetResource(id);
  DLOG_ASSERT(resource != nullptr);

  auto modelMatrix = resource->GetModel()->GetModelMatrix();

  if (m_showMove && m_showRotate && m_showScale) {
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix),
                         ImGuizmo::OPERATION::UNIVERSAL, ImGuizmo::LOCAL,
                         glm::value_ptr(modelMatrix));
  } else if (m_showMove) {
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix),
                         ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL,
                         glm::value_ptr(modelMatrix));
  } else if (m_showRotate) {
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix),
                         ImGuizmo::OPERATION::ROTATE, ImGuizmo::LOCAL, glm::value_ptr(modelMatrix));
  } else if (m_showScale) {
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix),
                         ImGuizmo::OPERATION::SCALE, ImGuizmo::LOCAL, glm::value_ptr(modelMatrix));
  }

  resource->GetModel()->SetModelMatrix(modelMatrix);
}

void
RenderImage::DrawLightManipulate() {
  const auto camera = m_scene->GetEditorCamrea();
  const auto viewMatrix = camera->GetViewMatrix();
  const auto perspectiveMatrix = camera->GetProjectionMatrix();

  auto entity = m_entity.value();
  if (Entity::HasComponent<PointLightComponent>(m_scene.get(), entity)) {
    auto& light = Entity::GetComponent<PointLightComponent>(m_scene.get(), entity).m_light;
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0), light.GetPos());
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix),
                         ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL,
                         glm::value_ptr(modelMatrix));
    auto newPos = glm::vec3(glm::column(modelMatrix, 3));
    light.SetPos(newPos);
  } else if (Entity::HasComponent<ParallelLightComponent>(m_scene.get(), entity)) {
    auto& light = Entity::GetComponent<ParallelLightComponent>(m_scene.get(), entity).m_light;
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0), light.GetPos());
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix),
                         ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL,
                         glm::value_ptr(modelMatrix));
    auto newPos = glm::vec3(glm::column(modelMatrix, 3));
    light.SetPos(newPos);
  }
}

}  // namespace Marbas
