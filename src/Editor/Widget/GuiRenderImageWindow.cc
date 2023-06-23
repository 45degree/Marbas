#include "GuiRenderImageWindow.hpp"

#include <IconsFontAwesome6.h>
#include <ImGuizmo.h>
#include <imgui_internal.h>

#include "Core/Scene/SceneManager.hpp"
#include "GuiComponent.hpp"

namespace Marbas::Gui {

void
RenderImageWidget::Manipulate(Scene* scene) {
  if (!ImGui::IsWindowFocused()) return;

  auto& io = ImGui::GetIO();

  static float m_mouseLastX = 0;
  static float m_mouseLastY = 0;

  auto [x, y] = io.MousePos;
  auto xOffset = x - m_mouseLastX;
  auto yOffset = y - m_mouseLastY;

  auto editorCamera = scene->GetEditorCamrea();
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
RenderImageWidget::OnDraw() {
  auto sceneManager = SceneManager::GetInstance();
  auto activeScene = sceneManager->GetActiveScene();

  if (m_image == std::nullopt) return;

  Manipulate(activeScene);

  /**
   * push the render result into the image
   */
  auto imageSize = ImGui::GetContentRegionAvail();
  // auto ImagePos = ImGui::GetCursorPos();
  auto ImagePos = ImGui::GetCursorScreenPos();
  ImGui::Image(*m_image, imageSize, ImVec2(0, 0), ImVec2(1, 1));

  // set imguizmo draw array
  auto camera = activeScene->GetEditorCamrea();

  ImGuizmo::SetOrthographic(false);
  ImGuizmo::SetDrawlist();
  ImGuizmo::SetRect(ImagePos.x, ImagePos.y, imageSize.x, imageSize.y);

  auto viewMatrix = camera->GetViewMatrix();
  auto ViewManipulateSize = ImVec2(100, 100);
  auto ViewManipulatePos = ImVec2(ImagePos.x + imageSize.x - ViewManipulateSize.x, ImagePos.y);
  ImGuizmo::ViewManipulate(glm::value_ptr(viewMatrix), camera->GetDistance(), ViewManipulatePos, ViewManipulateSize,
                           0x4FFFFFFF);
  camera->SetViewMatrix(viewMatrix);

  // draw
  auto& world = activeScene->GetWorld();
  if (world.any_of<ModelSceneNode>(m_zmoEntity) || world.any_of<EmptySceneNode>(m_zmoEntity)) {
    DrawModelManipulate(activeScene, m_zmoEntity, camera.get());
  } else if (world.any_of<DirectionalLightSceneNode>(m_zmoEntity)) {
    DrawLightManipulate(activeScene, m_zmoEntity, camera.get());
  } else if (world.any_of<VXGIProbeSceneNode>(m_zmoEntity)) {
    DrawLightManipulate(activeScene, m_zmoEntity, camera.get());
  }

  /**
   * set view matrix for next render
   */
  float aspect = imageSize.x / imageSize.y;
  camera->SetAspect(aspect);
};

void
RenderImageWidget::DrawModelManipulate(Scene* scene, entt::entity entity, Camera* camera) {
  using enum ImGuizmo::OPERATION;
  using enum ImGuizmo::MODE;

  const auto viewMatrix = camera->GetViewMatrix();
  auto perspectiveMatrix = camera->GetProjectionMatrix();
  perspectiveMatrix[1][1] *= -1;  // flip the y axis because the vulkan coordinate is different from the imguizmo

  auto& world = scene->GetWorld();
  if (!world.any_of<TransformComp>(entity)) return;

  auto modelMatrix = world.get<TransformComp>(entity).GetLocalTransform();

  auto& showMove = m_zmoDrawInfo.showMove;
  auto& showRotate = m_zmoDrawInfo.showRotate;
  auto& showScale = m_zmoDrawInfo.showScale;
  auto* viewMatrixPtr = glm::value_ptr(viewMatrix);
  auto* projMatrixPtr = glm::value_ptr(perspectiveMatrix);
  auto* modelMatrixPtr = glm::value_ptr(modelMatrix);

  bool changeValue = false;
  if (showMove && showRotate && showScale) {
    changeValue |= ImGuizmo::Manipulate(viewMatrixPtr, projMatrixPtr, UNIVERSAL, LOCAL, modelMatrixPtr);
  } else if (showMove) {
    changeValue |= ImGuizmo::Manipulate(viewMatrixPtr, projMatrixPtr, TRANSLATE, LOCAL, modelMatrixPtr);
  } else if (showRotate) {
    changeValue |= ImGuizmo::Manipulate(viewMatrixPtr, projMatrixPtr, ROTATE, LOCAL, modelMatrixPtr);
  } else if (showScale) {
    changeValue |= ImGuizmo::Manipulate(viewMatrixPtr, projMatrixPtr, SCALE, LOCAL, modelMatrixPtr);
  }

  if (changeValue) {
    scene->Update<TransformComp>(entity, [&](TransformComp& component) {
      component.SetLocalTransform(modelMatrix);
      return true;
    });
  }
}

void
RenderImageWidget::DrawLightManipulate(Scene* scene, entt::entity entity, Camera* camera) {
  const auto viewMatrix = camera->GetViewMatrix();
  auto perspectiveMatrix = camera->GetProjectionMatrix();
  perspectiveMatrix[1][1] *= -1;  // flip the y axis because the vulkan coordinate is different from the imguizmo

  auto& world = scene->GetWorld();
  if (!world.any_of<TransformComp>(entity)) return;
  auto modelMatrix = world.get<TransformComp>(entity).GetLocalTransform();

  auto* viewMatrixPtr = glm::value_ptr(viewMatrix);
  auto* projMatrixPtr = glm::value_ptr(perspectiveMatrix);
  auto* modelMatrixPtr = glm::value_ptr(modelMatrix);
  ImGuizmo::Manipulate(viewMatrixPtr, projMatrixPtr, ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, modelMatrixPtr);

  scene->Update<TransformComp>(entity, [&](TransformComp& component) {
    component.SetLocalTransform(modelMatrix);
    return true;
  });
}

/**
 * render image tool bar
 */
void
RenderImageToolBar::OnDraw() {
  auto sceneManager = SceneManager::GetInstance();
  auto activeScene = sceneManager->GetActiveScene();
  auto& world = activeScene->GetWorld();

  // auto view = world.view<GuizmoComponent>();

  ImGui::PushID(0);
  ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f, 0.));

  ImguiZmoDrawInfo zmoDrawInfo;
  if (ImGui::Button(ICON_FA_ARROW_POINTER)) {
    zmoDrawInfo.showMove = true;
    zmoDrawInfo.showRotate = true;
    zmoDrawInfo.showScale = true;
    ChangeZmoDrawInfoSign.Publish(zmoDrawInfo);
  }

  ImGui::SameLine();
  ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT)) {
    zmoDrawInfo.showMove = true;
    zmoDrawInfo.showRotate = false;
    zmoDrawInfo.showScale = false;
    ChangeZmoDrawInfoSign.Publish(zmoDrawInfo);
  }

  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_ARROW_ROTATE_LEFT)) {
    zmoDrawInfo.showMove = false;
    zmoDrawInfo.showRotate = true;
    zmoDrawInfo.showScale = false;
    ChangeZmoDrawInfoSign.Publish(zmoDrawInfo);
  }

  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_MAXIMIZE)) {
    zmoDrawInfo.showMove = false;
    zmoDrawInfo.showRotate = false;
    zmoDrawInfo.showScale = true;
    ChangeZmoDrawInfoSign.Publish(zmoDrawInfo);
  }

  ImGui::SameLine();
  ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

  ImGui::SameLine();

  // focus the view point
  if (ImGui::Button(ICON_FA_ARROWS_TO_DOT)) {
    if (world.any_of<ModelSceneNode>(m_focusEntity)) {
      auto camera = activeScene->GetEditorCamrea();
      auto perspectiveMatrix = camera->GetProjectionMatrix();
      auto modelMatrix = world.get<TransformComp>(m_focusEntity).GetGlobalTransform();

      float modelPos[3], unused1[3], unused2[3];
      ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix), modelPos, unused1, unused2);
      auto cameraPos = camera->GetPosition();
      camera->MovePosition(glm::vec3(modelPos[0], modelPos[1], modelPos[2]));
    }
  }

  ImGui::SameLine();
  ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

  ImGui::PopStyleColor(1);
  ImGui::PopID();
}

void
RenderImageWindow::OnSelectedEntity(Scene& scene, entt::entity entity) {
  auto& world = scene.GetWorld();
  ImguiZmoDrawInfo zmoDrawInfo;

  if (entity == entt::null) {
    zmoDrawInfo.showMove = false;
    zmoDrawInfo.showRotate = false;
    zmoDrawInfo.showScale = false;
  } else if (world.any_of<EmptySceneNode>(entity)) {
    zmoDrawInfo.showMove = false;
    zmoDrawInfo.showRotate = false;
    zmoDrawInfo.showScale = false;
  } else if (world.any_of<ModelSceneNode>(entity)) {
    zmoDrawInfo.showMove = true;
    zmoDrawInfo.showRotate = true;
    zmoDrawInfo.showScale = true;
  } else if (world.any_of<DirectionalLightSceneNode>(entity)) {
    zmoDrawInfo.showMove = true;
    zmoDrawInfo.showRotate = false;
    zmoDrawInfo.showScale = false;
  }
  m_imageWidget.SetGuizmoDrawInfo(zmoDrawInfo);
  m_imageWidget.SetGuizmoEntity(scene, entity);
  m_toolBar.SetFocusEntity(scene, entity);
}

}  // namespace Marbas::Gui
