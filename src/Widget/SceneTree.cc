#include "Widget/SceneTree.h"

#include <imgui.h>

namespace Marbas {

static void DrawNode(SceneNode* node) {
    if(node == nullptr) return;

    if(node->GetSubSceneNodesCount() > 0) {
        if(ImGui::TreeNode(node->GetSceneNodeName())) {
            auto subNodes = node->GetSubSceneNodes();
            for(int i = 0; i < node->GetSubSceneNodesCount(); i++) {
                DrawNode(subNodes[i]);
            }
            ImGui::TreePop();
        }
    }
    else ImGui::Selectable(node->GetSceneNodeName());
}

void SceneTreeWidget::Draw() {
    if(m_scene == nullptr) return;

    auto* sceneRoot = m_scene->GetRootSceneNode();
    DrawNode(sceneRoot);
}

}  // namespace Marbas
