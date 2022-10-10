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
  if (!Entity::HasComponent<HierarchyComponent>(m_scene.get(), entity)) {
    return;
  }
  auto& hierarchyComponent = Entity::GetComponent<HierarchyComponent>(m_scene.get(), entity);

  String name = "default node name";
  EntityType type;
  if (Entity::HasComponent<UniqueTagComponent>(m_scene.get(), entity)) {
    const auto& tag = Entity::GetComponent<UniqueTagComponent>(m_scene.get(), entity);
    name = tag.tagName;
    type = tag.type;
  }

  if (type == EntityType::Mesh) return;  // not draw mesh node

  if (ImGui::TreeNode(name.c_str())) {
    if (ImGui::IsItemClicked()) {
      m_signal.publish(entity);
    }

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

  String name;
  if (Entity::HasComponent<UniqueTagComponent>(m_scene.get(), m_PopupEntity)) {
    const auto& tag = Entity::GetComponent<UniqueTagComponent>(m_scene.get(), m_PopupEntity);
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
      FileDialog fileDialog("obj,pmx,FBX");
      fileDialog.Open(".");
      if (fileDialog.GetResult().has_value()) {
        auto modelPath = fileDialog.GetResult().value();
        auto modelResourceContainer = m_resourceManager->GetModelResourceContainer();
        auto modelResource = modelResourceContainer->CreateResource(modelPath);
        auto modelResourceId = modelResourceContainer->AddResource(modelResource);
        modelResource->LoadResource(m_rhiFactory, m_resourceManager.get());

        m_scene->AddModel(modelResourceId, modelResource->GetModel()->GetModelName(),
                          m_PopupEntity);
      }
    }

    if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES " Add BillBoard")) {
      auto textureResourceContainer = m_resourceManager->GetTexture2DResourceContainer();
      auto resource = textureResourceContainer->CreateResource("assert/lightbulb.png");
      auto id = textureResourceContainer->AddResource(resource);
      m_scene->AddBillBoard(id, glm::vec3(0, 0, 0), m_PopupEntity);
    }

    if (ImGui::BeginMenu(ICON_FA_CIRCLE_NODES "Add Light")) {
      if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES "Add Point Light")) {
        m_scene->AddLight(LightType::PointLight, glm::vec3(0, 0, 0), m_PopupEntity);
      }
      if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES "Add direction Light")) {
        m_scene->AddLight(LightType::ParalleLight, glm::vec3(0, 0, 0), m_PopupEntity);
      }
      ImGui::EndMenu();
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
