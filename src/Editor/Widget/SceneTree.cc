#include "SceneTree.hpp"

#include <IconsFontAwesome6.h>
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

  // String name;
  // if (Entity::HasComponent<UniqueTagComponent>(m_scene, m_PopupEntity)) {
  //   const auto& tag = Entity::GetComponent<UniqueTagComponent>(m_scene, m_PopupEntity);
  //   name = tag.tagName;
  // }

  auto& world = m_scene->GetWorld();
  if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
    ImGui::OpenPopup("scenePopup");
  }
  if (ImGui::BeginPopup("scenePopup")) {
    // ImGui::Text("%s", name.c_str());
    ImGui::Separator();

    if (ImGui::MenuItem("empty node")) {
      // m_scene->AddEmptyNode(m_PopupEntity);
      auto node = m_scene->AddChild(m_PopupEntity);
      world.emplace<EmptySceneNode>(node);
    }

    if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES " Add Model")) {
      auto node = m_scene->AddChild(m_PopupEntity);
      world.emplace<ModelSceneNode>(node);

      // Add Model to the scene
      // FileDialog fileDialog("obj,pmx,FBX,blend");
      // fileDialog.Open(".");
      // if (fileDialog.GetResult().has_value()) {
      //   auto modelPath = fileDialog.GetResult().value();
      //
      //   auto node = m_scene->AddChild(m_PopupEntity);
      //   world.emplace<ModelSceneNode>(node);
      //   // auto modelNodeEntity = m_scene->AddModelNode(m_PopupEntity);
      //   auto& modelNode = m_scene->GetWorld().get<ModelSceneNode>(node);
      //   // modelNode.modelPath =
      //   // modelNode.SetModel(m_scene->GetWorld(), modelPath, m_rhiFactory, m_resourceManager);
      // }
    }

    if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES " Add BillBoard")) {
      // auto textureResourceContainer = m_resourceManager->GetTexture2DResourceContainer();
      // auto resource = textureResourceContainer->CreateResource("assert/lightbulb.png");
      // auto id = textureResourceContainer->AddResource(resource);
      // m_scene->AddBillBoard(id, glm::vec3(0, 0, 0), m_PopupEntity);
    }

    if (ImGui::BeginMenu(ICON_FA_CIRCLE_NODES "Add Light")) {
      // if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES "Add Point Light")) {
      //   m_scene->AddLight(LightType::PointLight, glm::vec3(0, 0, 0), m_PopupEntity);
      // }
      if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES "Add direction Light")) {
        auto node = m_scene->AddChild(m_PopupEntity);
        world.emplace<DirectionalLightSceneNode>(node);
        // auto lightNodeEntity = m_scene->AddLightNode(m_PopupEntity);
        // auto& lightNode = m_scene->GetWorld().get<LightSceneNode>(lightNodeEntity);
        // lightNode.SetLight(LightType::ParalleLight, m_scene->GetWorld());
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
