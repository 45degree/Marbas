#include "Widget/SceneTree.hpp"

#include <imgui.h>
#include <glog/logging.h>

namespace Marbas {

void SceneTreeWidget::DrawNode(const SceneNode* node) {
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
        if (ImGui::Selectable(node->GetSceneNodeName())) {
            m_renderImage->SetSelectedSceneNode(const_cast<SceneNode*>(node));
        }
    }
}

void SceneTreeWidget::Draw() {
    if(m_scene == nullptr) return;

    auto* sceneRoot = m_scene->GetRootSceneNode();
    DrawNode(sceneRoot);
}

}  // namespace Marbas
