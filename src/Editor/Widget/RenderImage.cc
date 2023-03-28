#include "RenderImage.hpp"

#include <IconsFontAwesome6.h>
#include <ImGuizmo.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <iostream>

#include "Common/Common.hpp"
#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Editor/Application.hpp"
#include "RHIFactory.hpp"

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
  auto& world = m_scene->GetWorld();
  if (ImGui::Button(ICON_FA_ARROWS_TO_DOT)) {
    if (m_entity != entt::null) {
      if (world.any_of<ModelSceneNode>(m_entity)) {
        auto entity = m_entity;
        auto camera = m_scene->GetEditorCamrea();
        auto perspectiveMatrix = camera->GetProjectionMatrix();
        auto modelMatrix = world.get<TransformComp>(entity).GetGlobalTransform();

        float modelPos[3], unused1[3], unused2[3];
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix), modelPos, unused1, unused2);
        auto cameraPos = camera->GetPosition();
        camera->MovePosition(glm::vec3(modelPos[0], modelPos[1], modelPos[2]));
      }
    }
  }

  ImGui::SameLine();
  ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_ARROW_UP)) {
    needBakeScene = true;
  }

  ImGui::PopStyleColor(1);
  ImGui::PopID();
}

void
RenderImage::Manipulate() {
  if (!ImGui::IsWindowFocused()) return;

  auto& io = ImGui::GetIO();

  static float m_mouseLastX = 0;
  static float m_mouseLastY = 0;

  auto [x, y] = io.MousePos;
  auto xOffset = x - m_mouseLastX;
  auto yOffset = y - m_mouseLastY;

  auto editorCamera = m_scene->GetEditorCamrea();
  if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
      auto right = editorCamera->GetRightVector();  // x-axis in camera coordinate
      auto pos = editorCamera->GetPosition();
      auto up = editorCamera->GetUpVector();

      // The movement of the viewpoint is always opposite to the mouse, and the direction of the
      // v-axis in the image coordinate system is also opposite to the direction of the y-axis in
      // the camera coordinate, so the y-axis coordinate calculation does not need to take a
      // negative sign.
      auto moveOffset = -xOffset * right + yOffset * up;
      editorCamera->MoveFixPoint(moveOffset.x, moveOffset.y, moveOffset.z);
    } else if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
      editorCamera->AddPitch(yOffset);
      editorCamera->AddYaw(-xOffset);
    }
  }

  m_mouseLastX = x;
  m_mouseLastY = y;

  // scroll
  editorCamera->AddDistance(io.MouseWheel * 10);
};

void
RenderImage::Draw() {
  Manipulate();
  ShowToolBar();

  /**
   * push the render result into the image
   */
  imageSize = ImGui::GetContentRegionAvail();
  auto ImagePos = ImGui::GetCursorPos();
  ImGui::Image(m_renderImage, imageSize, ImVec2(0, 1), ImVec2(1, 0));

  /**
   *
   */

  // set imguizmo draw array
  auto winPos = ImGui::GetWindowPos();
  auto camera = m_scene->GetEditorCamrea();

  // ImVec2 vMax = ImGui::GetWindowContentRegionMax();

  ImGuizmo::SetOrthographic(false);
  ImGuizmo::SetDrawlist();
  ImGuizmo::SetRect(winPos.x + ImagePos.x, winPos.y + ImagePos.y, imageSize.x, imageSize.y);

  auto viewMatrix = camera->GetViewMatrix();
  auto ViewManipulateSize = ImVec2(100, 100);
  auto ViewManipulatePos = ImVec2(winPos.x + ImagePos.x + imageSize.x - ViewManipulateSize.x, winPos.y + ImagePos.y);
  ImGuizmo::ViewManipulate(glm::value_ptr(viewMatrix), camera->GetDistance(), ViewManipulatePos, ViewManipulateSize,
                           0x4FFFFFFF);
  camera->SetViewMatrix(viewMatrix);

  // draw
  // TODO: handle all entity type
  auto& world = m_scene->GetWorld();
  if (m_entity != entt::null) {
    if (world.any_of<ModelSceneNode>(m_entity) || world.any_of<EmptySceneNode>(m_entity)) {
      DrawModelManipulate();
    } else if (world.any_of<DirectionalLightSceneNode>(m_entity)) {
      DrawLightManipulate();
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

  auto modelMatrix = m_scene->GetWorld().get<TransformComp>(m_entity).GetGlobalTransform();

  if (m_showMove && m_showRotate && m_showScale) {
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix), ImGuizmo::OPERATION::UNIVERSAL,
                         ImGuizmo::LOCAL, glm::value_ptr(modelMatrix));
  } else if (m_showMove) {
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix), ImGuizmo::OPERATION::TRANSLATE,
                         ImGuizmo::LOCAL, glm::value_ptr(modelMatrix));
  } else if (m_showRotate) {
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix), ImGuizmo::OPERATION::ROTATE,
                         ImGuizmo::LOCAL, glm::value_ptr(modelMatrix));
  } else if (m_showScale) {
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix), ImGuizmo::OPERATION::SCALE,
                         ImGuizmo::LOCAL, glm::value_ptr(modelMatrix));
  }

  m_scene->GetWorld().get<TransformComp>(m_entity).SetGlobalTransform(modelMatrix);
}

void
RenderImage::DrawLightManipulate() {
  const auto camera = m_scene->GetEditorCamrea();
  const auto viewMatrix = camera->GetViewMatrix();
  const auto perspectiveMatrix = camera->GetProjectionMatrix();

  // auto& modelMatrix = Entity::GetComponent<HierarchyComponent>(m_scene, m_entity).globalTransformMatrix;
  auto modelMatrix = m_scene->GetWorld().get<TransformComp>(m_entity).GetGlobalTransform();

  ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix), ImGuizmo::OPERATION::TRANSLATE,
                       ImGuizmo::LOCAL, glm::value_ptr(modelMatrix));

  m_scene->GetWorld().get<TransformComp>(m_entity).SetGlobalTransform(modelMatrix);
  // HierarchyComponent::ResetGlobalTransformMatrix(m_scene->GetWorld(), m_entity, modelMatrix);

  // if (Entity::HasComponent<PointLightComponent>(m_scene, m_entity)) {
  //   auto& light = Entity::GetComponent<PointLightComponent>(m_scene, m_entity).m_light;
  //   glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0), light.GetPos());
  //   ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix),
  //   ImGuizmo::OPERATION::TRANSLATE,
  //                        ImGuizmo::LOCAL, glm::value_ptr(modelMatrix));
  //   auto newPos = glm::vec3(glm::column(modelMatrix, 3));
  //   light.SetPos(newPos);
  // } else if (Entity::HasComponent<ParallelLightComponent>(m_scene, m_entity)) {
  //   auto& light = Entity::GetComponent<ParallelLightComponent>(m_scene, m_entity).m_light;
  //   glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0), light.GetPos());
  //   ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(perspectiveMatrix),
  //   ImGuizmo::OPERATION::TRANSLATE,
  //                        ImGuizmo::LOCAL, glm::value_ptr(modelMatrix));
  //   auto newPos = glm::vec3(glm::column(modelMatrix, 3));
  //   light.SetPos(newPos);
  // }
}

}  // namespace Marbas
