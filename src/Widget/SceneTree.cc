#include "Widget/SceneTree.hpp"

#include <IconsFontAwesome6.h>
#include <glog/logging.h>
#include <imgui.h>

#include "Core/Entity.hpp"

namespace Marbas {

void SceneTreeWidget::DrawNode(const SceneNode* node) {
  if (node == nullptr) return;

  if (ImGui::TreeNode(node->GetSceneNodeName())) {
    auto& meshes = node->GetMeshes();
    for (auto& mesh : meshes) {
      auto& tagsComponent = Entity::GetComponent<TagsCompoment>(m_scene, mesh);
      const auto& tagName = tagsComponent.tags[TagsKey::NAME];
      String name = String(ICON_FA_CIRCLE_NODES) + tagName.c_str();
      if (ImGui::Selectable(name.c_str())) {
        for (auto* widget : m_selectMeshWidgets) {
          widget->ChangeMesh(mesh);
        }
      }
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
      ImGui::OpenPopup("scenePopup");
    }

    if (ImGui::BeginPopup("scenePopup")) {
      ImGui::Text("%s", node->GetSceneNodeName());
      if (ImGui::MenuItem("Add Model")) {
        m_modeladdDialog->Open();
        m_selectedNode = const_cast<SceneNode*>(node);
      }
      if (node == m_scene->GetRootSceneNode()) {
        if (ImGui::MenuItem("Change SkyBox")) {
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
}

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
    // auto scene = Scene::CreateSceneFromFile(filePathName, m_resourceManager);
    // auto renderLayer = m_window->GetRenderLayer();
    // renderLayer->SetSecne(std::move(scene));
  });
}

}  // namespace Marbas
