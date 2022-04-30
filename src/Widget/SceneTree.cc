#include "Widget/SceneTree.hpp"
#include "Core/Entity.hpp"

#include <IconsFontAwesome6.h>

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
        // if (ImGui::Selectable(node->GetSceneNodeName())) {
        //     m_renderImage->SetSelectedSceneNode(const_cast<SceneNode*>(node));
        // }
    }

    auto& meshes =node->GetMeshes();
    for(auto& mesh : meshes) {
        auto& tagsComponent = Entity::GetComponent<TagsCompoment>(m_scene, mesh);
        const auto& tagName = tagsComponent.tags[TagsKey::NAME];
        String name = String(ICON_FA_CIRCLE_NODES) + tagName.c_str();
        if(ImGui::Selectable(name.c_str())) {
            m_renderImage->SetSelectedSceneNode(mesh);
        }
    }
}

void SceneTreeWidget::Draw() {
    if(m_scene == nullptr) return;

    auto* sceneRoot = m_scene->GetRootSceneNode();
    DrawNode(sceneRoot);
}

}  // namespace Marbas
