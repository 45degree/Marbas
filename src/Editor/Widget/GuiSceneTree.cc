#include "GuiSceneTree.hpp"

#include <IconsFontAwesome6.h>

#include "CommonName.hpp"
#include "Core/Scene/SceneManager.hpp"

namespace Marbas::Gui {

std::string
GetNodeName(entt::registry& world, entt::entity entity) {
  std::string name = "default node name##";
  if (world.any_of<EmptySceneNode>(entity)) {
    auto& emptyNode = world.get<EmptySceneNode>(entity);
    name = "EmptyNode##" + std::to_string((uint32_t)entity);
  } else if (world.any_of<ModelSceneNode>(entity)) {
    auto& modelNode = world.get<ModelSceneNode>(entity);
    name = "Model##" + std::to_string((uint32_t)entity);
  } else if (world.any_of<DirectionalLightSceneNode>(entity)) {
    auto& lightNode = world.get<DirectionalLightSceneNode>(entity);
    name = lightNode.nodeName + "##" + std::to_string((uint32_t)entity);
  }
  return name;
}

void
GuiSceneTree::OnDraw() {
  auto sceneManager = SceneManager::GetInstance();
  m_scene = sceneManager->GetActiveScene();

  auto rootEntity = m_scene->GetRootNode();
  DrawNode(rootEntity);

  if (ImGui::BeginDragDropTarget()) {
    if (auto* payload = ImGui::AcceptDragDropPayload(CONTENT_BROWSER_DRAGDROG); payload != nullptr) {
      const auto& path = *reinterpret_cast<AssetPath*>(payload->Data);
      // TODO: check if the path is a scene path
      auto sceneManager = SceneManager::GetInstance();
      sceneManager->LoadScene(path);

      auto scene = sceneManager->GetScene(path);
      sceneManager->SetActiveScene(scene);
      SelectEntitySign.Publish(*scene, entt::null);
    }
    ImGui::EndDragDropTarget();
  }
}

void
GuiSceneTree::DrawNode(entt::entity entity) {
  if (entity == entt::null) return;

  auto& world = m_scene->GetWorld();
  if (!world.any_of<HierarchyComponent>(entity)) {
    return;
  }
  auto& hierarchyComponent = world.get<HierarchyComponent>(entity);

  ImGuiTreeNodeFlags flags =
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
  if (entity == m_selectNode) {
    flags |= ImGuiTreeNodeFlags_Selected;
  }

  if (hierarchyComponent.children.empty()) {
    flags |= ImGuiTreeNodeFlags_Leaf;
  }

  bool nodeOpen = ImGui::TreeNodeEx(GetNodeName(world, entity).c_str(), flags);
  if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
    m_selectNode = entity;
    SelectEntitySign.Publish(*m_scene, m_selectNode);
  }

  if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
    ImGui::OpenPopup("scenePopup");
  }

  if (ImGui::BeginPopup("scenePopup")) {
    if (ImGui::MenuItem("empty node")) {
      auto node = m_scene->AddChild(entity);
      world.emplace<EmptySceneNode>(node);
    }

    if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES " Add Model")) {
      auto node = m_scene->AddChild(entity);
      world.emplace<ModelSceneNode>(node);
    }

    if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES " Add BillBoard")) {
    }

    if (ImGui::BeginMenu(ICON_FA_LIGHTBULB " Add Light")) {
      if (ImGui::MenuItem(ICON_FA_LIGHTBULB " Add direction Light")) {
        auto node = m_scene->AddChild(entity);
        world.emplace<DirectionalLightSceneNode>(node);
      }
      ImGui::EndMenu();
    }

    ImGui::EndPopup();
  }

  if (nodeOpen) {
    auto children = hierarchyComponent.children;
    for (auto& subEntity : children) {
      DrawNode(subEntity);
    }
    ImGui::TreePop();
  }
}

}  // namespace Marbas::Gui
