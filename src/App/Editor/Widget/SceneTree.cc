#include "App/Editor/Widget/SceneTree.hpp"

#include <IconsFontAwesome6.h>
#include <glog/logging.h>
#include <imgui.h>

#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"

namespace Marbas {

String
AddIconForStringByTag(const char* str, const UniqueTagComponent& tag) {
  const auto& type = tag.type;
  String result;
  switch (type) {
    case EntityType::Mesh:
      result = String(ICON_FA_CIRCLE_NODES " ") + str;
      break;
    default:
      result = str;
  }

  return result;
}

void
SceneTreeWidget::DrawNode(const entt::entity& entity) {
  if (entity == entt::null) return;
  if (!Entity::HasComponent<HierarchyComponent>(m_scene, entity)) {
    return;
  }
  auto& hierarchyComponent = Entity::GetComponent<HierarchyComponent>(m_scene, entity);

  String name = "default node name";
  EntityType type;
  if (Entity::HasComponent<UniqueTagComponent>(m_scene, entity)) {
    const auto& tag = Entity::GetComponent<UniqueTagComponent>(m_scene, entity);
    name = tag.tagName;
    type = tag.type;
  }

  if (type == EntityType::Mesh) return;

  if (ImGui::TreeNode(name.c_str())) {
    if (ImGui::IsItemClicked() && type == EntityType::Model) {
      m_signal.publish(entity, Entity::GetComponent<ModelComponent>(m_scene, entity));
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
      m_selectEntity = entity;
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
  if (m_selectEntity == entt::null) return;

  String name;
  if (Entity::HasComponent<UniqueTagComponent>(m_scene, m_selectEntity)) {
    const auto& tag = Entity::GetComponent<UniqueTagComponent>(m_scene, m_selectEntity);
    name = tag.tagName;
  }

  if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
    ImGui::OpenPopup("scenePopup");
  }
  if (ImGui::BeginPopup("scenePopup")) {
    ImGui::Text("%s", name.c_str());
    ImGui::Separator();

    if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES " Add Model")) {
      // Add Model to the scene
      FileDialog fileDialog("obj,pmx");
      fileDialog.Open(".");
      if (fileDialog.GetResult().has_value()) {
        auto modelPath = fileDialog.GetResult().value();
        auto modelResourceContainer = m_resourceManager->GetModelResourceContainer();
        auto modelResource = modelResourceContainer->CreateResource(modelPath);
        auto modelResourceId = modelResourceContainer->AddResource(modelResource);
        modelResource->LoadResource(m_rhiFactory, m_resourceManager);

        m_scene->AddModel(modelResourceId, modelResource->GetModel()->GetModelName(),
                          m_selectEntity);
      }
    }

    ImGui::EndPopup();
  }
}

void
SceneTreeWidget::Draw() {
  if (m_scene == nullptr) return;

  auto rootEntity = m_scene->GetRootEntity();
  DrawNode(rootEntity);
  DrawPopup();
}

SceneTreeWidget::SceneTreeWidget(RHIFactory* rhiFactory) : Widget("SceneTree", rhiFactory) {}

}  // namespace Marbas
