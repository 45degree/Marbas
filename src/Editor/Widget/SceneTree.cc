#include "SceneTree.hpp"

#include <IconsFontAwesome6.h>
#include <IconsMaterialDesign.h>
#include <glog/logging.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <unicode/ucnv.h>
#include <unicode/ucsdet.h>

#include <iostream>

#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Editor/Widget/AssetSelectDialog.hpp"

namespace Marbas {

void
SceneTreeWidget::DrawNode(const entt::entity& entity, uint32_t indent) {
  if (entity == entt::null) return;

  auto& world = m_scene->GetWorld();
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
    auto& lightNode = m_scene->GetWorld().get<DirectionalLightSceneNode>(entity);
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
    for (const auto& subEntity : children) {
      DrawNode(subEntity);
    }

    ImGui::TreePop();
  }
}

void
SceneTreeWidget::DrawPopup() {
  if (m_PopupEntity == entt::null) return;

  auto& world = m_scene->GetWorld();
  if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
    ImGui::OpenPopup("scenePopup");
  }

  // change style
  if (ImGui::BeginPopup("scenePopup")) {
    if (ImGui::MenuItem("empty node")) {
      auto node = m_scene->AddChild(m_PopupEntity);
      world.emplace<EmptySceneNode>(node);
    }

    if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES " Add Model")) {
      auto node = m_scene->AddChild(m_PopupEntity);
      world.emplace<ModelSceneNode>(node);
    }

    if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES " Add BillBoard")) {
    }

    if (ImGui::BeginMenu(ICON_FA_LIGHTBULB " Add Light")) {
      if (ImGui::MenuItem(ICON_FA_LIGHTBULB " Add direction Light")) {
        auto node = m_scene->AddChild(m_PopupEntity);
        world.emplace<DirectionalLightSceneNode>(node);
      }
      ImGui::EndMenu();
    }

    ImGui::EndPopup();
  }
}

void
SceneTreeWidget::Draw() {
  if (m_scene == nullptr) return;

  auto rootEntity = m_scene->GetRootNode();
  DrawNode(rootEntity);
  DrawPopup();
}

SceneTreeWidget::SceneTreeWidget(RHIFactory* rhiFactory, Scene* scene)
    : Widget("SceneTree", rhiFactory), m_scene(scene) {}

}  // namespace Marbas
