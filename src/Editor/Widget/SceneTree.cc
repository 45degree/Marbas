#include "SceneTree.hpp"

#include <IconsFontAwesome6.h>
#include <IconsMaterialDesign.h>
#include <glog/logging.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <unicode/ucnv.h>
#include <unicode/ucsdet.h>

#include <iostream>

#include "Core/Scene/Component/Component.hpp"
#include "Core/Scene/SceneManager.hpp"
#include "Editor/Widget/AssetSelectDialog.hpp"
#include "Editor/Widget/CommonName.hpp"

namespace Marbas {

void
SceneTreeWidget::DrawNode(Scene* scene, entt::entity& entity, uint32_t indent) {
  if (entity == entt::null) return;

  auto& world = scene->GetWorld();
  if (!world.any_of<HierarchyComponent>(entity)) {
    return;
  }
  auto& hierarchyComponent = world.get<HierarchyComponent>(entity);

  String name = "default node name##";
  if (world.any_of<EmptySceneNode>(entity)) {
    auto& emptyNode = world.get<EmptySceneNode>(entity);
    name = "EmptyNode##" + std::to_string((uint32_t)entity);
  } else if (world.any_of<ModelSceneNode>(entity)) {
    auto& modelNode = world.get<ModelSceneNode>(entity);
    name = "Model##" + std::to_string((uint32_t)entity);
  } else if (world.any_of<DirectionalLightSceneNode>(entity)) {
    auto& lightNode = scene->GetWorld().get<DirectionalLightSceneNode>(entity);
    name = lightNode.nodeName + "##" + std::to_string((uint32_t)entity);
  }

  bool isOpen = ImGui::TreeNode(name.c_str());
  if (ImGui::IsItemClicked()) {
    m_selectEntity.Publish(entity);
  }

  if (isOpen) {
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
      m_PopupEntity = entity;
    }

    auto children = hierarchyComponent.children;
    for (auto& subEntity : children) {
      DrawNode(scene, subEntity);
    }

    ImGui::TreePop();
  }
}

void
SceneTreeWidget::DrawPopup() {
  if (m_PopupEntity == entt::null) return;

  auto sceneManager = SceneManager::GetInstance();
  auto activeScene = sceneManager->GetActiveScene();
  auto& world = activeScene->GetWorld();
  if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
    ImGui::OpenPopup("scenePopup");
  }

  // change style
  if (ImGui::BeginPopup("scenePopup")) {
    if (ImGui::MenuItem("empty node")) {
      auto node = activeScene->AddChild(m_PopupEntity);
      world.emplace<EmptySceneNode>(node);
    }

    if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES " Add Model")) {
      auto node = activeScene->AddChild(m_PopupEntity);
      world.emplace<ModelSceneNode>(node);
    }

    if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES " Add BillBoard")) {
    }

    if (ImGui::BeginMenu(ICON_FA_LIGHTBULB " Add Light")) {
      if (ImGui::MenuItem(ICON_FA_LIGHTBULB " Add direction Light")) {
        auto node = activeScene->AddChild(m_PopupEntity);
        world.emplace<DirectionalLightSceneNode>(node);
      }
      ImGui::EndMenu();
    }

    ImGui::EndPopup();
  }
}

void
SceneTreeWidget::Draw() {
  auto sceneManager = SceneManager::GetInstance();
  auto activeScene = sceneManager->GetActiveScene();

  auto rootEntity = activeScene->GetRootNode();
  DrawNode(activeScene, rootEntity);
  DrawPopup();
}

SceneTreeWidget::SceneTreeWidget(RHIFactory* rhiFactory) : Widget("SceneTree", rhiFactory) {}

}  // namespace Marbas
