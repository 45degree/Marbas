#include "Widget/SceneTree.hpp"

#include <imgui.h>

namespace Marbas {

static void DrawNode(const SceneNode* node) {
    if(node == nullptr) return;

    auto subNodes = node->GetSubSceneNodes();
    if(!subNodes.empty()) {
        if(ImGui::TreeNode(node->GetSceneNodeName())) {
            auto subNodes = node->GetSubSceneNodes();
            for(const auto* subNode : subNodes) {
                DrawNode(subNode);
            }
            ImGui::TreePop();
        }
    }
    else {
        ImGui::Selectable(node->GetSceneNodeName());
        for(int i = 0; i < node->GetMeshCount(); i++) {
            auto mesh = node->GetMesh(i);
            ImGui::Text("------------------");
            ImGui::Text("%s", mesh->GetMeshName().c_str());
            auto textures = mesh->GetTexture();
            for(auto texture : textures) {
                ImGui::Text("%s", texture->GetImagePath().c_str());
            }
        }
    }
}

void SceneTreeWidget::Draw() {
    if(m_scene == nullptr) return;

    auto* sceneRoot = m_scene->GetRootSceneNode();
    DrawNode(sceneRoot);
}

}  // namespace Marbas
