#include "Widget/SceneTree.hpp"

#include <imgui.h>

namespace Marbas {

static void DrawNode(const SceneNode* node) {
    if(node == nullptr) return;

    auto subNodes = node->GetSubSceneNodes();
    if(!subNodes.empty()) {
        if(ImGui::TreeNode(node->GetSceneNodeName())) {
            for(const auto* subNode : subNodes) {
                DrawNode(subNode);
            }
            ImGui::TreePop();
        }
    }
    else {
        ImGui::Selectable(node->GetSceneNodeName());
    }
}

void SceneTreeWidget::Draw() {
    if(m_scene == nullptr) return;

    auto* sceneRoot = m_scene->GetRootSceneNode();
    DrawNode(sceneRoot);
}

}  // namespace Marbas
