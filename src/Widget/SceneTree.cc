#include "Widget/SceneTree.hpp"

#include <IconsFontAwesome6.h>
#include <glog/logging.h>
#include <imgui.h>

#include "Core/Entity.hpp"

namespace Marbas {

String AddIconForStringByTag(const char* str, const TagsCompoment& tag) {
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

void SceneTreeWidget::DrawScene() {
  auto* rootNode = m_scene->GetRootSceneNode();
  if (ImGui::TreeNode(rootNode->GetSceneNodeName())) {
  }
}

void SceneTreeWidget::DrawNode(const SceneNode* node) {
  if (node == nullptr) return;

  if (ImGui::TreeNode(node->GetSceneNodeName())) {
    auto& entities = node->GetEntities();
    for (auto& entity : entities) {
      auto& tagsComponent = Entity::GetComponent<TagsCompoment>(m_scene, entity);
      const auto& tagName = tagsComponent.name;
      const auto& type = tagsComponent.type;

      String name = AddIconForStringByTag(tagName.c_str(), tagsComponent);
      if (ImGui::Selectable(name.c_str())) {
        if (type == EntityType::Mesh) {
          for (auto* widget : m_selectMeshWidgets) {
            widget->ChangeMesh(entity);
          }
        }
      }
    }

    // root node
    if (node == m_scene->GetRootSceneNode()) {
      auto skyBox = m_scene->GetSkyBox();
      if (ImGui::Selectable(String(ICON_FA_CUBE " skyBox").c_str())) {
      }
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
      ImGui::OpenPopup("scenePopup");
    }

    if (ImGui::BeginPopup("scenePopup")) {
      ImGui::Text("%s", node->GetSceneNodeName());
      ImGui::Separator();

      if (ImGui::MenuItem("Add Model")) {
        m_modeladdDialog->Open();
        m_selectedNode = const_cast<SceneNode*>(node);
      }
      if (node == m_scene->GetRootSceneNode()) {
        if (ImGui::MenuItem("Change SkyBox")) {
          // TODO change cubemap
        }
      }

      ImGui::EndPopup();
    }

    auto subNodes = node->GetSubSceneNodes();
    for (const auto* subNode : subNodes) {
      DrawNode(subNode);
    }

    ImGui::TreePop();
  }
}  // namespace Marbas

void SceneTreeWidget::Draw() {
  if (m_scene == nullptr) return;

  auto* sceneRoot = m_scene->GetRootSceneNode();
  DrawNode(sceneRoot);

  m_modeladdDialog->Draw();
}

SceneTreeWidget::SceneTreeWidget() : Widget("SceneTree") {
  FileDialogCrateInfo info{
      "TextureOpenDialog",
      "Open a texture",
      "model file (*.off *.obj *.ply *.pmx){.off,.obj,.ply,.pmx}",
  };
  m_modeladdDialog = std::make_unique<FileDialog>(info);

  m_modeladdDialog->SelectCallback([&](const char* filePathName, const char* fileName) {
    auto subNode = SceneNode::ReadModelFromFile(filePathName, m_scene, m_resourceManager);
    m_selectedNode->AddSubSceneNode(std::move(subNode));
    m_selectedNode = nullptr;
  });
}

}  // namespace Marbas
